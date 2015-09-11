#
#	Build Sandix Kernel
#
#	Copyright (C) 2015 Yizhou Shan <shanyizhou@ict.ac.cn>
#
#	This program is free software; you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation; either version 2 of the License, or
#	(at your option) any later version.
#
#	This program is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License along
#	with this program; if not, write to the Free Software Foundation, Inc.,
#	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

VERSION		=	1
MINORLEVEL	=	0
PATCHLEVEL	=	0
NAME		=	Sandix

# o Do NOT use built-in rules and variables
#   This increases performance and avoids hard-to-debug behaviour );
# o Look for make include files relative to root of kernel src
MAKEFLAGS += -rR --include-dir=$(CURDIR)

# Avoid funny character set dependencies
unexport LC_ALL
LC_COLLATE=C
LC_NUMERIC=C
export LC_COLLATE LC_NUMERIC

# Avoid interference with shell env settings
unexport GREP_OPTIONS

# Beautify output
# ===========================================================================
# Normally, we echo the whole command before executing it. By making
# that echo $($(quiet)$(cmd)), we now have the possibility to set
# $(quiet) to choose other forms of output instead, e.g.
#
#         quiet_cmd_cc_o_c = Compiling $(RELDIR)/$@
#         cmd_cc_o_c       = $(CC) $(c_flags) -c -o $@ $<
#
# If $(quiet) is empty, the whole command will be printed.
# If it is set to "quiet_", only the short version will be printed.
# If it is set to "silent_", nothing will be printed at all, since
# the variable $(silent_cmd_cc_o_c) doesn't exist.
#
# A simple variant is to prefix commands with $(Q) - that's useful
# for commands that shall be hidden in non-verbose mode.
#
#	$(Q)ln $@ :<
#
# If KBUILD_VERBOSE equals 0 then the above command will be hidden.
# If KBUILD_VERBOSE equals 1 then the above command is displayed.
#
# To put more focus on warnings, be less verbose as default
# Use 'make V=1' to see the full commands
ifeq ("$(origin V)", "command line")
  KBUILD_VERBOSE=$(V)
endif

ifndef KBUILD_VERBOSE
  KBUILD_VERBOSE=0
endif

ifeq ($(KBUILD_VERBOSE),1)
  quiet=
  Q=
else
  quiet=quiet_
  Q=@
endif

# If the user is running "make -s", suppress echoing of commands
ifneq ($(filter 4.%,$(MAKE_VERSION)),)	# make-4
ifneq ($(filter %s ,$(firstword x$(MAKEFLAGS))),)
  quiet=silent_
endif
else					# make-3.8x
ifneq ($(filter s% -s%,$(MAKEFLAGS)),)
  quiet=silent_
endif
endif

export quiet Q KBUILD_VERBOSE

# kbuild supports saving output files in a separate directory.
# To locate output files in a separate directory two syntaxes are supported.
# In both cases the working directory must be the root of the kernel src.
#
# 1) O=
# Use "make O=dir/to/store/output/files/"
#
# 2) Set KBUILD_OUTPUT
# Set the environment variable KBUILD_OUTPUT to point to the directory
# where the output files shall be placed.
# export KBUILD_OUTPUT=dir/to/store/output/files/
# make
#
# The O= assignment takes precedence over the KBUILD_OUTPUT environment
# variable.
#
# KBUILD_SRC is set on invocation of make in OBJ directory
# KBUILD_SRC is not intended to be used by the regular user (for now)


# ===========================================================================
#	First Part of the Makefile	
# ===========================================================================

# OK, Make called in directory where kernel src resides
# Do we want to locate output files in a separate directory?
ifeq ($(KBUILD_SRC),)
ifeq ("$(origin O)", "command line")
  KBUILD_OUTPUT := $(O)
endif

# The default target!
PHONY := _all
_all:

# Cancel implicit rules on top Makefile
$(CURDIR)/Makefile Makefile: ;

ifneq ($(KBUILD_OUTPUT),)
saved-output := $(KBUILD_OUTPUT)
KBUILD_OUTPUT := $(shell mkdir -p $(KBUILD_OUTPUT)	&& \
			 cd $(KBUILD_OUTPUT)		&& \
			 /bin/pwd)
$(if $(KBUILD_OUTPUT),, \
     $(error Failed to create output directory "$(saved-output)"))

PHONY += $(MAKECMDGOALS) sub-make
$(filter-out _all sub-make $(CURDIR)/Makefile, $(MAKECMDGOALS)) _all: sub-make
	@:

sub-make: FORCE
	$(Q)$(MAKE) -C $(KBUILD_OUTPUT) KBUILD_SRC=$(CURDIR) \
	-f $(CURDIR)/Makefile $(filter-out _all sub-make,$(MAKECMDGOALS))

# Leave processing to sub-make
skip-makefile := 1
endif # ifneq ($(KBUILD_OUTPUT),)
endif # ifeq ($(KBUILD_SRC),)


# ===========================================================================
#	Second Part of the Makefile	
# ===========================================================================

# Process the rest of the Makefile if this is the final invocation.
ifeq ($(skip-makefile),)

PHONY += all
_all: outputmakefile

# Do not print "Entering directory ...",
# but we want to display it when entering to the output directory
# so that IDEs/editors are able to understand relative filenames.
MAKEFLAGS += --no-print-directory

# Call a source code checker (by default, "sparse") as part of the
# C compilation.
#
# Use 'make C=1' to enable checking of only re-compiled files.
# Use 'make C=2' to enable checking of *all* source files, regardless
# of whether they are re-compiled or not.
#
# See the file "Documentation/sparse.txt" for more details, including
# where to get the "sparse" utility.

ifeq ("$(origin C)", "command line")
  KBUILD_CHECKSRC = $(C)
endif
ifndef KBUILD_CHECKSRC
  KBUILD_CHECKSRC = 0
endif
export KBUILD_CHECKSRC

ifeq ($(KBUILD_SRC),)
  srctree := .
else
  srctree := $(KBUILD_SRC)
endif
objtree		:= .
src		:= $(srctree)
obj		:= $(objtree)
export srctree objtree
export KBUILD_SRC

# Cross compiling and selecting different set of gcc/bin-utils
# CROSS_COMPILE specify the prefix used for all executables used
# during compilation. Only gcc and related bin-utils executables
# are prefixed with $(CROSS_COMPILE).

# FIXME
HOST_ARCH := $(shell uname -m | sed -e s/i.86/x86/ -e s/x86_64/x86/ \
				  -e s/sun4u/sparc64/ \
				  -e s/arm.*/arm/ -e s/sa110/arm/ \
				  -e s/s390x/s390/ -e s/parisc64/parisc/ \
				  -e s/ppc.*/powerpc/ -e s/mips.*/mips/ \
				  -e s/sh[234].*/sh/ -e s/aarch64.*/arm64/ )
ifneq ("$(HOST_ARCH)", "i386")
  CROSS_COMPILE	:= i386-elf-
else
  CROSS_COMPILE	:=
endif
export CROSS_COMPILE HOST_ARCH

# Sandix has x86 ARCH only
# Hmm, maybe use i386 instead.
SRCARCH := x86
export SRCARCH

# ARCH specific headers
hdr-arch := $(SRCARCH)

KCONFIG_CONFIG ?= .config
export KCONFIG_CONFIG

# SHELL used by kbuild
CONFIG_SHELL := $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
	  else if [ -x /bin/bash ]; then echo /bin/bash; \
	  else echo sh; fi ; fi)

HOSTCC       = gcc
HOSTCXX      = g++
HOSTCFLAGS   = -std=gnu11 -Wall -Wmissing-prototypes -Wstrict-prototypes -O2 -fomit-frame-pointer
HOSTCXXFLAGS = -O2

ifeq ($(shell $(HOSTCC) -v 2>&1 | grep -c "clang version"), 1)
HOSTCFLAGS  += -Wno-unused-value -Wno-unused-parameter \
		-Wno-missing-field-initializers -fno-delete-null-pointer-checks
endif
export CONFIG_SHELL HOSTCC HOSTCXX HOSTCFLAGS HOSTCXXFLAGS

# Generic definitions
scripts/Kbuild.include: ;
include scripts/Kbuild.include

CC			 = $(CROSS_COMPILE)gcc
AS			 = $(CROSS_COMPILE)as
LD			 = $(CROSS_COMPILE)ld
CPP			 = $(CC) -E
AR			 = $(CROSS_COMPILE)ar
NM			 = $(CROSS_COMPILE)nm
STRIP			 = $(CROSS_COMPILE)strip
OBJCOPY			 = $(CROSS_COMPILE)objcopy
OBJDUMP			 = $(CROSS_COMPILE)objdump
MAKE			 = make
AWK			 = awk
PERL			 = perl
PYTHON			 = python
CHECK			 = sparse

CHECKFLAGS		:= -D__sandix__ -Dsandix

NOSTDINC_FLAGS		:= -nostdinc

SANDIXINCLUDE		:=							\
			-Iinclude						\
			$(if $(KBUILD_SRC), -I$(srctree)/include)		\
			-I$(srctree)/arch/$(hdr-arch)/include

KBUILD_CFLAGS   	:=
			-Wall -Wundef -Wstrict-prototypes -Wno-trigraphs	\
			-fno-strict-aliasing -fno-common			\
			-Werror-implicit-function-declaration			\
			-Wno-format-security					\
			-std=gnu11

KBUILD_CPPFLAGS		:= -D__KERNEL__
KBUILD_AFLAGS		:= -D__ASSEMBLY__
KBUILD_LDFLAGS		:=

# FIXME
# Ugly.. You can add more, but DO NOT DELETE them. 
OBJCOPYFLAGS	:= -j .text -j .text32 -j .data -j .rodata -j .init -O binary
OBJDUMPFLAGS	:= -d -M att

export VERSION PATCHLEVEL SUBLEVEL NAME
export CC AS LD CPP AR NM STRIP OBJCOPY OBJDUMP
export MAKE AWK PERL PYTHON CHECK CHECKFLAGS
export SANDIXINCLUDE NOSTDINC_FLAGS
export KBUILD_CFLAGS KBUILD_CPPFLAGS KBUILD_LDFLAGS KBUILD_AFLAGS
export OBJCOPYFLAGS OBJDUMPFLAGS


# ===========================================================================
# Rules shared between *config targets and build targets

# Basic helpers built in scripts/
PHONY += scripts_basic
scripts_basic:
	$(Q)$(MAKE) $(build)=scripts/basic

# To avoid any implicit rule to kick in, define an empty command.
scripts/basic/%: scripts_basic ;

# Generates a Makefile in the output directory, if using a
# separate output directory. This allows convenient use of
# make in the output directory.
PHONY += outputmakefile
outputmakefile:
ifneq ($(KBUILD_SRC),)
	$(Q)ln -fsn $(srctree) source
	$(Q)$(CONFIG_SHELL) $(srctree)/scripts/mkmakefile \
	    $(srctree) $(objtree) $(VERSION) $(PATCHLEVEL)
endif

# Handle when user mixing *config targets and build targets.
# For example 'make oldconfig all'.
# Detect when mixed targets is specified, and make a second invocation
# of make so .config is not included in this case either (for *config).
ifneq ($(filter config %config,$(MAKECMDGOALS)),)
  config-targets := 1
  ifneq ($(words $(MAKECMDGOALS)),1)
    mixed-targets := 1
  endif
else
  config-targets := 0
  mixed-targets  := 0
endif

# ===========================================================================
# We're called with mixed targets (*config and build targets).
# Handle them one by one.
ifeq ($(mixed-targets),1)

PHONY += $(MAKECMDGOALS) __build_one_by_one
$(filter-out __build_one_by_one, $(MAKECMDGOALS)): __build_one_by_one
	@:

__build_one_by_one:
	$(Q)set -e;					\
	for i in $(MAKECMDGOALS); do			\
		$(MAKE) -f $(srctree)/Makefile $$i;	\
	done

else

ifeq ($(config-targets),1)
# ===========================================================================
# *config targets only

include arch/$(SRCARCH)/Makefile
export KBUILD_DEFCONFIG KBUILD_KCONFIG

config:  scripts_basic outputmakefile FORCE
	$(Q)$(MAKE) $(build)=scripts/kconfig $@

%config: scripts_basic outputmakefile FORCE
	$(Q)$(MAKE) $(build)=scripts/kconfig $@

else
# ===========================================================================
# Build targets only - this includes vmlinux, arch specific targets, clean
# targets and others. In general all targets except *config targets.

boot-y		:= boot/
init-y		:= init/
core-y		:= kernel/ mm/ lib/
drivers-y	:= drivers/
vmsandix-dirs	:= $(patsubst %/, %, $(boot-y) $(init-y) $(core-y) $(drivers-y))

boot-y		:= $(patsubst %/, %/built-in.o, $(boot-y))
init-y		:= $(patsubst %/, %/built-in.o, $(init-y))
core-y		:= $(patsubst %/, %/built-in.o, $(core-y))
drivers-y	:= $(patsubst %/, %/built-in.o, $(drivers-y))
vmsandix-deps	:= $(boot-y) $(init-y) $(core-y) $(drivers-y)

KBUILD_VMSANDIX_BOOT := $(boot-y)
KBUILD_VMSANDIX_MAIN := $(init-y) $(core-y) $(drivers-y)

export KBUILD_VMSANDIX_BOOT KBUILD_VMSANDIX_MAIN

all: bzImage vmsandix

# The arch Makefile can set ARCH_{CPP,A,C}FLAGS to override the default
# values of the respective KBUILD_* variables
ARCH_CPPFLAGS :=
ARCH_AFLAGS :=
ARCH_CFLAGS :=
include arch/$(SRCARCH)/Makefile

endif #ifeq ($(config-targets),1)
endif #ifeq ($(mixed-targets),1)








#
#	Fine, one architecture only.
#
CONFIG_X86_32=y
ifeq ($(CONFIG_X86_32),y)
    KBUILD_AFLAGS += -m32
	
    KBUILD_CFLAGS += -m32

    KBUILD_CFLAGS += -mregparm=3 -freg-struct-return

    # Never want PIC in kernel
    KBUILD_CFLAGS += -fno-pic

    # Prevent gcc from keeping the stack 16 byte aligned
    KBUILD_CFLAGS += -mpreferred-stack-boundary=2
    
    # Prevent gcc from generating any FP code by mistake
    KBUILD_CFLAGS += -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx
    
    # __STDC_HOSTED = 0;
    # GCC __builtin_ still works, but you need to invoke __builtin_
    # functions manually. See <sandix/compiler.h> for more details.
    KBUILD_CFLAGS += -ffreestanding
endif


#
#	BIG FAT NOTE:
#	o Real-Mode kernel image is boot/rmimage.
#	o Link $(init-y) $(core-y) $(drivers-y) together to form protected-mode
#	  kernel image. Move and rename the pm kernel image to boot/pmimage.
#	o boot/CATENATE is used to catenate bootloader, rmimage and
#	  pmimage together to form bzImage.
#
_RM_IMAGE := boot/rmimage
_PM_IMAGE := boot/pmimage
RM_IMAGE  := boot/rmimage.bin
PM_IMAGE  := boot/pmimage.bin
VMSANDIX  := boot/vmsandix
BZIMAGE   := boot/bzImage
RM_LD_CMD := boot/rm-image.ld
PM_LD_CMD := kernel/vmSandix.ld


#
#	COMMANDS FOR BUILD BZIMAGE
#
quiet_cmd_link_rm := LD $(SS)  $(_RM_IMAGE)
      cmd_link_rm := $(LD) -T $(RM_LD_CMD) -o $(_RM_IMAGE) $(KBUILD_VMSANDIX_BOOT)

quiet_cmd_link_pm := LD $(SS)  $(_PM_IMAGE)
      cmd_link_pm := $(LD) -T $(PM_LD_CMD) -o $(_PM_IMAGE) $(KBUILD_VMSANDIX_MAIN)

quiet_cmd_bin_rm := OBJCOPY $(RM_IMAGE)
      cmd_bin_rm := $(OBJCOPY) $(OBJCOPYFLAGS) $(_RM_IMAGE) $(RM_IMAGE)

quiet_cmd_bin_pm := OBJCOPY $(PM_IMAGE)
      cmd_bin_pm := $(OBJCOPY) $(OBJCOPYFLAGS) $(_PM_IMAGE) $(PM_IMAGE)

quiet_cmd_catenate := CAT$(SS) $(VMSANDIX)
      cmd_catenate := boot/CATENATE

quiet_cmd_map := SYSTEM MAP
      cmd_map := $(NM) -n $(_PM_IMAGE) > boot/System.map

#
#	BUILD KERNEL
# ===========================================================================

bzImage: vmsandix
	@chmod +x $(VMSANDIX)
	@mv $(VMSANDIX) $(BZIMAGE)

# FIXME boot/ should be separated from deps
vmsandix: $(vmsandix-deps)
	$(call if_changed,link_pm)
	$(call if_changed,link_rm)
	$(call if_changed,bin_pm)
	$(call if_changed,bin_rm)
	$(call if_changed,map)
	$(call if_changed,catenate)

$(sort $(vmsandix-deps)): $(vmsandix-dirs) ;

# Descending into sub-directory
PHONY += $(vmsandix-dirs)
$(vmsandix-dirs):
	$(Q)$(MAKE) $(BUILD)=$@

#
#	CLEAN
# ===========================================================================

# Trick: add prefix to avoid overriding the previous targets.
CLEAN_DIRS := $(addprefix __CLEAN__,$(vmsandix-dirs))

PHONY += clean
clean: $(CLEAN_DIRS)
	@rm -f $(_RM_IMAGE) $(RM_IMAGE)
	@rm -f $(_PM_IMAGE) $(PM_IMAGE)
	@rm -f $(VMSANDIX) $(BZIMAGE)
	@rm -f boot/System.map

$(CLEAN_DIRS):
	$(Q)$(MAKE) $(CLEAN)=$(patsubst __CLEAN__%,%,$@)

#
#	HELP INSTRUCTIONS
# ===========================================================================
PHONY += help
help:
	@echo "\n###################################################"
	@echo "\nBUILD SANDIX KERNEL\n"
	@echo "  make V=0 [targets] => Quiet Build (default)"
	@echo "  make V=1 [targets] => Verbose Build"
	@echo "  make C=0 [targets] => Do _Not_ Check Source Code Before Build (default)"
	@echo "  make C=1 [targets] => Check Source Code Before Build"
	@echo "\n###################################################"

endif  # ifeq ($(skip-makefile),)

PHONY += FORCE
FORCE:

#
#	There are two reasons to use a phony target:
#	1) to avoid a conflict with a file of the same name,
#	2) to improve performance.
# ===========================================================================
.PHONY: $(PHONY)
