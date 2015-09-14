#
#	Build Sandix Kernel
#
#	For more information, please read samples/Makefile.tutorial
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
#	Note that:
#	arch/x86/Makefile Plays an important role.
#	1st Part:	Argument Processing/External Sub-Make
#	2st Part:	Variable Initialization/Preprocessing
#	3st Part:	Targets Definitions/Relationships

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


# ===========================================================================
#  1st	First Part of the Makefile	
# ===========================================================================
#
# The first part handles externel building. When users want to build on other
# directory, the first part normally invokes a sub-make to do this.

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

# OK, Make called in directory where kernel src resides
# Do we want to locate output files in a separate directory?
ifeq ($(KBUILD_SRC),)
ifeq ("$(origin O)", "command line")
  KBUILD_OUTPUT := $(O)
endif

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


# Process The Rest of Makefile if this is the final invocation.
# A reminder, The Rest means from here to the end of Makefile. ;)
ifeq ($(skip-makefile),)

# ===========================================================================
#  2st	Second Part of the Makefile	
# ===========================================================================
#
# The second part complete all the preprocessing and variable initialization.
# We leave the make targets processing to the third part.

_all: all

# Do not print "Entering directory ..."
# But we want to display it when entering to the output directory.
MAKEFLAGS += --no-print-directory

ifeq ($(KBUILD_SRC),)
  srctree := .
else
  srctree := $(KBUILD_SRC)
endif
objtree		:= .
src		:= $(srctree)
obj		:= $(objtree)
export KBUILD_SRC srctree objtree

# Cross compiling and selecting different set of gcc/bin-utils
# CROSS_COMPILE specify the prefix used for all executables used
# during compilation. Only gcc and related bin-utils executables
# are prefixed with $(CROSS_COMPILE).
HOST_ARCH := $(shell uname -m | sed -e s/i.86/x86/ -e s/x86_64/x86/ \
				  -e s/sun4u/sparc64/ \
				  -e s/arm.*/arm/ -e s/sa110/arm/ \
				  -e s/s390x/s390/ -e s/parisc64/parisc/ \
				  -e s/ppc.*/powerpc/ -e s/mips.*/mips/ \
				  -e s/sh[234].*/sh/ -e s/aarch64.*/arm64/ )
# TODO
# Sandix has x86 ARCH only
# Hmm, maybe use i386 instead.
ifneq ("$(HOST_ARCH)", "i386")
  CROSS_COMPILE	:= i386-elf-
else
  CROSS_COMPILE	:=
endif

ARCH	:= x86
SRCARCH	:= x86
export SRCARCH ARCH HOST_ARCH CROSS_COMPILE

KCONFIG_CONFIG ?= .config
export KCONFIG_CONFIG

# Shell used by kbuild
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

##
# Generic definitions
#
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
KBUILD_CPPFLAGS		:= -D__KERNEL__
KBUILD_AFLAGS		:= -D__ASSEMBLY__
KBUILD_LDFLAGS		:=
KBUILD_ARFLAGS		:=
OBJCOPYFLAGS		:=
OBJDUMPFLAGS		:= -d -M att
#OBJCOPYFLAGS := -j .text -j .text32 -j .data -j .rodata -j .init -O binary

SANDIXINCLUDE		:=							\
			   -Iinclude						\
			   $(if $(KBUILD_SRC), -I$(srctree)/include)		\
			   -I$(srctree)/arch/$(SRCARCH)/include

KBUILD_CFLAGS   	:=							\
			   -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs	\
			   -fno-strict-aliasing -fno-common			\
			   -Werror-implicit-function-declaration		\
			   -Wno-format-security					\
			   -std=gnu11

export VERSION PATCHLEVEL SUBLEVEL NAME
export CC AS LD CPP AR NM STRIP OBJCOPY OBJDUMP
export MAKE AWK PERL PYTHON CHECK CHECKFLAGS
export SANDIXINCLUDE NOSTDINC_FLAGS
export KBUILD_CFLAGS KBUILD_CPPFLAGS KBUILD_LDFLAGS KBUILD_AFLAGS
export KBUILD_ARFLAGS OBJCOPYFLAGS OBJDUMPFLAGS

# ===========================================================================
#   3st	Third Part of the Makefile	
# ===========================================================================
#
# The third part defines all the targets and their relationships
# Note that: All_The_Build_And_Config_Targets. ;)
#
# List of Targets:
#
#	scipts_basic:
#	outputmakefile:
#	config
#	%config
#	scripts
#	all
#	vmSandix
#	clean
#	help

##
# Basic helpers built in scripts/
#
PHONY += scripts_basic
scripts_basic:
	$(Q)$(MAKE) $(build)=scripts/basic

# To avoid any implicit rule to kick in, define an empty command.
scripts/basic/%: scripts_basic ;

##
# Generates a Makefile in the output directory, if using a
# separate output directory. This allows convenient use of
# make in the output directory.
#
PHONY += outputmakefile
outputmakefile:
ifneq ($(KBUILD_SRC),)
	$(Q)ln -fsn $(srctree) source
	$(Q)$(CONFIG_SHELL) $(srctree)/scripts/mkmakefile.sh \
	    $(srctree) $(objtree) $(VERSION) $(PATCHLEVEL)
endif

# ===========================================================================
# Handle mixed targets one by one.
# Detect when mixed targets is specified, and make a second invocation
# of make so .config is not included in this case either (for *config).
# For example:
#	make defconfig all
#	make oldconfig help
ifneq ($(filter config %config,$(MAKECMDGOALS)),)
        config-targets := 1
        ifneq ($(words $(MAKECMDGOALS)),1)
                mixed-targets := 1
        endif
else
        config-targets := 0
        mixed-targets  := 0
endif

ifeq ($(mixed-targets),1)

PHONY += $(MAKECMDGOALS) build_one_by_one
$(filter-out build_one_by_one, $(MAKECMDGOALS)): build_one_by_one
	@:

build_one_by_one:
	$(Q)set -e;					\
	for i in $(MAKECMDGOALS); do			\
		$(MAKE) -f $(srctree)/Makefile $$i;	\
	done

else

ifeq ($(config-targets),1)
# ===========================================================================
# Config targets only
# e.g.
#	make config
#	make menuconfig
#	...
# ===========================================================================

KBUILD_DEFCONFIG	:=
KBUILD_KCONFIG		:=
include arch/$(SRCARCH)/Makefile
export KBUILD_DEFCONFIG KBUILD_KCONFIG

config:  scripts_basic outputmakefile FORCE
	$(Q)$(MAKE) $(build)=scripts/kconfig $@

%config: scripts_basic outputmakefile FORCE
	$(Q)$(MAKE) $(build)=scripts/kconfig $@

else
# ===========================================================================
# Build targets only. All targets except *Config targets.
# e.g.
#	make all
#	make clean
#	...
# ===========================================================================

# TODO
##
# Additional Helper scripts
#
PHONY += scripts
scripts: scripts_basic
	@:

##
# The all: target is the default when no target is given on the command line.
# This allow a user to issue only 'make' to build Sandix kernel.
# Defaults to vmSandix, but the arch makefile usually adds further targets
#
PHONY += all
all: scripts_basic vmSandix

##
# Generally, final kernel image has 7 parts:
#	boot-y		-	Architecture Setup Part(Real-Mode in x86)
#	head-y		-	Kernel Image Header(Normally, head.S)
#	init-y		-	Kernel Init Part(init/*)
#	core-y		-	Kernel Built-in Part(e.g. kernel/ mm/)
#	libs-y		-	Kernel Library
#	net-y		-	Kernel Network Sub-system
#	drivers-y	-	Kernel Various Drivers
#
boot-y		:=
head-y		:=
init-y		:= init/
#core-y		:= kernel/ mm/
#libs-y		:= lib/
#net-y		:= net/
drivers-y	:= drivers/
ARCH_CPPFLAGS	:=
ARCH_AFLAGS	:=
ARCH_CFLAGS	:=

# Let arch Makefile overrides.
include arch/$(SRCARCH)/Makefile
# Add more Compiler and Linker flags.
include scripts/Makefile.flags

vmSandix-dirs	:= $(patsubst %/, %, $(init-y) $(core-y) $(libs-y) $(drivers-y))
init-y		:= $(patsubst %/, %/built-in.o, $(init-y))
core-y		:= $(patsubst %/, %/built-in.o, $(core-y))
libs-y		:= $(patsubst %/, %/built-in.o, $(libs-y))
net-y		:= $(patsubst %/, %/built-in.o, $(net-y))
drivers-y	:= $(patsubst %/, %/built-in.o, $(drivers-y))
vmSandix-deps	:= $(head-y) $(init-y) $(core-y) $(libs-y) $(net-y) $(drivers-y)

##
# Externally visible to link-vmSandix.sh
#
export KBUILD_VMSANDIX_INIT := $(head-y) $(init-y)
export KBUILD_VMSANDIX_MAIN := $(core-y) $(libs-y) $(net-y) $(drivers-y)
export KBUILD_VMSANDIX_LDS  := arch/$(SRCARCH)/kernel/vmSandix.ld

##
# Default kernel image to build.
#
export KBUILD_IMAGE ?= bzImage

##
# INSTALL_PATH specifies where to place the updated kernel and system map
# images. Default is /boot, but you can set it to other values
#
export INSTALL_PATH ?= /boot

##
# Final link of vmSandix
#
quiet_cmd_link-vmSandix = LINK    $@
      cmd_link-vmSandix = $(CONFIG_SHELL) $< LD
vmSandix: scripts/link-vmSandix.sh $(vmSandix-deps) FORCE
	$(call if_changed,link-vmSandix)

##
# The actual objects are generated when descending.
# Make sure no implicit rule kicks in
#
PHONY += $(vmSandix-dirs)
$(sort $(vmSandix-deps)): $(vmSandix-dirs) ;
$(vmSandix-dirs):
	$(Q)$(MAKE) $(build)=$@

###
# Clean
#
# Actually, we have only one clean target which deletes everything
# generated by building. It will deletes:
#	*.o	- Generated object file
#	.*.d	- Saved dependencies
#	.*.cmd	- Saved commands
#

# Add prefix to avoid overriding the previous targets.
clean-dirs := $(vmSandix-dirs) scripts/basic scripts/kconfig
clean-dirs := $(addprefix __clean__,$(clean-dirs))

PHONY += clean
clean: $(clean-dirs) arch-clean vmSandix-clean

# Clean files generated by linking vmSandix
PHONY += vmSandix-clean
vmSandix-clean:
	$(Q)$(CONFIG_SHELL) $(srctree)/scripts/link-vmSandix.sh clean

# Clean files by descending into sub-directories
PHONY += $(clean-dirs)
$(clean-dirs):
	$(Q)$(MAKE) $(clean)=$(patsubst __clean__%,%,$@)

PHONY += help
help:
	@echo  'Cleaning targets:'
	@echo  '  clean		  - Remove most generated files but keep the config and'
	@echo  '                    enough build support to build external modules'
	@echo  '  mrproper	  - Remove all generated files + config + various backup files'
	@echo  '  distclean	  - mrproper + remove editor backup and patch files'
	@echo  ''
	@echo  'Configuration targets:'
	@$(MAKE) -f $(srctree)/scripts/kconfig/Makefile help
	@echo  ''
	@echo  'Other generic targets:'
	@echo  '  all		  - Build all targets marked with [*]'
	@echo  '* bzImage         - Build the loadable kernel(with Bootloader, boot part)'
	@echo  '* vmSandix	  - Build the bare kernel'
	@echo  '  dir/            - Build all files in dir and below'
	@echo  '  dir/file.[oisS] - Build specified target only'
	@echo  ''
	@echo  'Building opinions:'
	@echo  '  make V=0   [targets] 0 => quiet build (default)'
	@echo  '  make V=1   [targets] 1 => verbose build'
	@echo  '  make V=2   [targets] 2 => give reason for rebuild of target'
	@echo  '  make O=dir [targets] Locate all output files in "dir", including .config'
	@echo  '  make C=1   [targets] Check all c source with $(CHECK) (sparse by default)'
	@echo  '  make C=2   [targets] Force check of all c source with $(CHECK)'
	@echo  '  make W=n   [targets] Enable extra gcc checks, n=1,2,3 where'
	@echo  '		1: warnings which may be relevant and do not occur too often'
	@echo  '		2: warnings which occur quite often but may still be relevant'
	@echo  '		3: more obscure warnings, can most likely be ignored'
	@echo  '		Multiple levels can be combined with W=12 or W=123'
	@echo  ''
	@echo  'Execute "make" or "make all" to build all targets marked with [*] '
	@echo  'For more information, please read samples/Makefile.tutorial'

##
# Generate Editor Tags
#
PHONY += tags
tags:
	@echo ''

##
# Generate Kernel Docs
#
PHONY += docs
docs:
	@echo ''

endif # ! ifeq ($(config-targets),1)
endif # ! ifeq ($(mixed-targets),1)
endif # ! ifeq ($(skip-makefile),)

PHONY += FORCE
FORCE:

.PHONY: $(PHONY)
