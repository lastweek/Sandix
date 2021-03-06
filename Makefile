#	Copyright (C) 2015-2016 Yizhou Shan <shan13@purdue.edu>
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

VERSION		=	3
PATCHLEVEL	=	1
SUBLEVEL	=	0
NAME		=	Sandix

# o Do NOT use built-in rules and variables
#   This increases performance and avoids hard-to-debug behaviour );
#   Yes, we will have very hard-to-debug errors!
# o Look for make include files relative to root of kernel src
MAKEFLAGS += -rR --include-dir=$(CURDIR)

# Avoid funny character set dependencies
unexport LC_ALL
LC_COLLATE=C
LC_NUMERIC=C
export LC_COLLATE LC_NUMERIC

# Avoid interference with shell env settings
unexport GREP_OPTIONS

# Colorful output
# ===========================================================================
# Normally, we echo everything in default color. By using the "make M=",
# we can now control the color of the output message.
# For example,
#		make M=1	--> red
#		make M=2 clean	--> green
#
# Note that M ranges from 0~7. We do NOT check for this.

ifeq ("$(origin M)", "command line")
  KBUILD_COLOR=$(M)
endif

ifndef KBUILD_COLOR
  COLOR_BEGIN=
  COLOR_END=
else
  COLOR_BEGIN=\033[3$(KBUILD_COLOR)m
  COLOR_END=\033[0m
endif

export KBUILD_COLOR COLOR_BEGIN COLOR_END

# Beautiful output
# ===========================================================================
# Normally, we echo the whole command before executing it. By making
# that echo $($(quiet)$(cmd)), we now have the possibility to set
# $(quiet) to choose other forms of output instead, e.g.
#
#         quiet_cmd_cc_o_c = Compiling $(RELDIR)/$@
#               cmd_cc_o_c = $(CC) $(c_flags) -c -o $@ $<
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

# Code checker
# ===========================================================================
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

#  1st	First Part of the Makefile	
# ===========================================================================
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

# KBUILD_SRC is set on invocation of make in OBJ directory
# KBUILD_SRC is not intended to be used by the regular user (for now)
ifeq ($(KBUILD_SRC),)

# OK, Make called in directory where kernel src resides
# Do we want to locate output files in a separate directory?
ifeq ("$(origin O)", "command line")
  $(error make O= Opinion not well suppoted)
  KBUILD_OUTPUT := $(O)
endif

# That's the default target when nothing is given on the command line
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

#  2st	Second Part of the Makefile	
# ===========================================================================
# The second part finish all the preprocessing and variable initialization.
# We leave the make-targets processing to the third part.

# Process the rest of Makefile if this is the final invocation.
ifeq ($(skip-makefile),)
_all: all

# Do not print "Entering directory ..."
# But we want to display it when entering to the output directory.
MAKEFLAGS += --no-print-directory

ifeq ($(KBUILD_SRC),)
  # Building in the src tree
  srctree := .
else
  # Building on external directory
  srctree := $(KBUILD_SRC)
endif

objtree		:= .
src		:= $(srctree)
obj		:= $(objtree)

export KBUILD_SRC srctree objtree

# Cross compiling and selecting different set of gcc/bin-utils
# ===========================================================================
# When performing cross compilation for other architectures ARCH shall be set
# to the target architecture. (See arch/* for the possibilities).
# ARCH can be set during invocation of make:
# make ARCH=x86
# Another way is to have ARCH set in the environment.
# The default ARCH is the host where make is executed.
#
# CROSS_COMPILE specify the prefix used for all executables used
# during compilation. Only gcc and related bin-utils executables
# are prefixed with $(CROSS_COMPILE).
#
# $(CROSS_COMPILE) can be set on the command line:
# make CROSS_COMPILE=i386-elf-
#
# Alternatively CROSS_COMPILE can be set in the environment.
# A third alternative is to store a setting in .config so that plain
# "make" in the configured kernel build directory always uses that.
# Default value for CROSS_COMPILE is not to prefix executables
# Note: Some architectures assign CROSS_COMPILE in their arch/*/Makefile

HOST_ARCH	:= $(shell uname -m | sed -e s/i.86/x86/ -e s/x86_64/x86/	\
				  	-e s/sun4u/sparc64/			\
				  	-e s/arm.*/arm/ -e s/sa110/arm/		\
				  	-e s/s390x/s390/ -e s/parisc64/parisc/	\
				  	-e s/ppc.*/powerpc/ -e s/mips.*/mips/	\
				  	-e s/sh[234].*/sh/ -e s/aarch64.*/arm64/)

ARCH		?= $(HOST_ARCH)
CROSS_COMPILE	?= $(CONFIG_CROSS_COMPILE:"%"=%)

#XXX
CROSS_COMPILE	:= i386-elf-

# Architecture as present in <generated/compile.h>
UTS_MACHINE	:= $(ARCH)
SRCARCH		:= $(ARCH)

# Additional ARCH settings for x86
ifeq ($(ARCH),i386)
        SRCARCH := x86
endif
ifeq ($(ARCH),x86_64)
        SRCARCH := x86
endif

export HOST_ARCH ARCH CROSS_COMPILE UTS_MACHINE SRCARCH

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

# Generic definitions; cancel implicit rules
scripts/Kbuild.include: ;
include scripts/Kbuild.include

# Make variables (CC, etc...)
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
OBJDUMPFLAGS		:= -M att

# It seems like if we run kconfig again, then a new <generated/autoconf.h>
# will be created, then all source files need to be re-compiled. However,
# fixdep.c would take care of this.
KCONFIGINCLUDE		:= -include $(srctree)/include/sandix/kconfig.h

SANDIXINCLUDE		:=							\
			   -Iinclude						\
			   -Iarch/$(SRCARCH)/include				\
			   $(if $(KBUILD_SRC), -I$(srctree)/include)		\
			   -I$(srctree)/arch/$(SRCARCH)/include			\
			   $(KCONFIGINCLUDE)


KBUILD_CFLAGS   	:=							\
			   -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs	\
			   -fno-strict-aliasing -fno-common			\
			   -Werror-implicit-function-declaration		\
			   -Wno-format-security					\
			   -std=gnu11

KERNELRELEASE		= $(shell cat include/config/kernel.release 2> /dev/null)
KERNELVERSION		= $(VERSION).$(PATCHLEVEL).$(SUBLEVEL)

export VERSION PATCHLEVEL SUBLEVEL KERNELRELEASE KERNELVERSION
export CC AS LD CPP AR NM STRIP OBJCOPY OBJDUMP MAKE AWK PERL PYTHON CHECK
export CHECKFLAGS NOSTDINC_FLAGS SANDIXINCLUDE
export KBUILD_CFLAGS KBUILD_CPPFLAGS KBUILD_LDFLAGS KBUILD_AFLAGS KBUILD_ARFLAGS 
export OBJCOPYFLAGS OBJDUMPFLAGS

# 3st	Third Part of the Makefile	
# ===========================================================================
# The third part describes the *config and build targets and their rules.

# Basic helpers
PHONY += scripts_basic
scripts_basic:
	$(Q)$(MAKE) $(build)=scripts/basic

# To avoid any implicit rule
scripts/basic/%: scripts_basic ;

# Generates a Makefile in the output directory, if using a
# separate output directory. This allows convenient use of
# make in the output directory.
PHONY += outputmakefile
outputmakefile:
ifneq ($(KBUILD_SRC),)
	$(Q)ln -fsn $(srctree) source
	$(Q)$(CONFIG_SHELL) $(srctree)/scripts/mkmakefile.sh \
	    $(srctree) $(objtree) $(VERSION) $(PATCHLEVEL)
endif

# To make sure we do not include .config for any of the *config targets.
# Catch them early, and hand them over to scripts/kconfig/Makefile.
# It is allowed to specify more targets when calling make, including
# mixing *config targets and build targets, e.g. "make config all".
# Detect when mixed targets is specified, and make a second invocation
# of make so .config is not included in this case either (for *config).

version_h	:= include/generated/version.h
utsrelease_h	:= include/generated/utsrelease.h

# Targets that do not need .config
no-dot-config-targets := clean mrproper distclean \
			 cscope gtags TAGS tags help% %docs check% coccicheck \
			 $(version_h) headers_% archheaders archscripts \
			 kernelversion %src-pkg

config-targets	:= 0
mixed-targets	:= 0
dot-config	:= 1

ifneq ($(filter $(no-dot-config-targets), $(MAKECMDGOALS)),)
       ifeq ($(filter-out $(no-dot-config-targets), $(MAKECMDGOALS)),)
               dot-config := 0
       endif
endif

ifneq ($(filter config %config,$(MAKECMDGOALS)),)
        config-targets := 1
        ifneq ($(words $(MAKECMDGOALS)),1)
                mixed-targets := 1
        endif
endif

ifeq ($(mixed-targets),1)
# ===========================================================================
# We are called with mixed targets(*config and build targets).
# Handle them one by one

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
# *config targets only - make sure prerequisites are updated,
# and descend in scripts/kconfig to make the *config target.

# Read arch specific Makefile to set KBUILD_DEFCONFIG as needed.
# KBUILD_DEFCONFIG may point out an alternative default configuration.
# Used for 'make defconfig'
KBUILD_DEFCONFIG	:=
KBUILD_KCONFIG		:=
include arch/$(SRCARCH)/Makefile
export KBUILD_DEFCONFIG KBUILD_KCONFIG

PHONY += config %config
config:  scripts_basic outputmakefile FORCE
	$(Q)$(MAKE) $(build)=scripts/kconfig $@

%config: scripts_basic outputmakefile FORCE
	$(Q)$(MAKE) $(build)=scripts/kconfig $@

else
# ===========================================================================
# Build targets only - this includes vmSanidx, arch specific targets, clean
# targets and others. In general, all targets except *config targets.

# Additional helpers built in scripts/
PHONY += scripts
scripts: scripts_basic include/config/auto.conf include/config/tristate.conf
	$(Q)$(MAKE) $(build)=$@

ifeq ($(dot-config),1)
# ===========================================================================
# We need .config file. And we must handle it properly. See comments below.

# read in config
-include include/config/auto.conf

# Read in dependencies to all Kconfig* files, make sure to
# run oldconfig if changes are detected.
-include include/config/auto.conf.cmd

# To avoid any implicit rule
$(KCONFIG_CONFIG) include/config/auto.conf.cmd: ;

# It is important to know how Makefiles are remade and how 'include'
# directive works. Here are some words form GNU Make Manual:
#
# If a Makefile can be remade from other files, you probably want make to
# get an up-to-date version of the Makefile to read in. After reading in all
# Makefiles, make will consider each as a goal target and attempt to update
# it. If a Makefile has a rule which says how to update it(found either in
# that very Makefile or in other one) or if an implicit rule applies to it,
# it will be updated if necessary. After all Makefiles have been checked, if
# any have actually been changed, make starts with a clean slate and reads all
# the Makefiles again.
#
# If you know that one or more of your Makefiles can NOT be remade and you want
# to keep make from performing an implicit rule search on them, perhaps for
# effiency reasons, you can use any normal method of preventing implicit rule
# look-up to do so. For example, you can write an explicit rule wuth the
# Makefile as the target, and an empty recipe.
#
# '-q' and '-n' do not prevent updating of Makefiles, because an out-of-date
# Makefile would result in the wrong output for other targets.
###
# Here, we have an explicit rule for included file include/config/auto.conf.
# So before GNUmake read "auto.conf", it will checkout the dependencies of it.
#
# That is why in a clean tree, this target always got built even you invoke with
# a non-exsit target, for example, "make ABCDEF".
#
# If .config is newer than include/config/auto.conf, someone tinkered with it
# and forgot to run "make oldconfig".
# If auto.conf.cmd is missing then we are probably in a cleaned tree so we
# execute the config step to be sure to catch updates.
include/config/%.conf: $(KCONFIG_CONFIG) include/config/auto.conf.cmd
	$(Q)$(MAKE) -f $(srctree)/Makefile silentoldconfig

else
-include include/config/auto.conf
# Dummy target needed, since it is used as prerequisite.
include/config/auto.conf: ;
endif # !$(dot-config)

# The all: target is the default when no target is given on the command line.
# This allow a user to issue only 'make' to build Sandix kernel.
# Defaults to vmSandix, but the arch makefile usually adds further targets
PHONY += all
all: scripts vmSandix

# bzImage = 
#	boot-y		-	Architecture setup image (realmode in x86)
# 	vmSandix	-	Kernel system image
#
# vmSandix = 
#	head-y		-	System image header (Normally, head.S)
#	init-y		-	Kernel init part
#	core-y		-	Kernel core part
#	libs-y		-	Kernel libraries
#	net-y		-	Kernel network subsystem
#	drivers-y	-	Kernel drivers

boot-y		:=
head-y		:=

init-y		:= init/
core-y		:= kernel/ mm/
libs-y		:= lib/
net-y		:= net/
drivers-y	:= drivers/

# The arch Makefile can set ARCH_{CPP,A,C}FLAGS to override the default
# values of the respective KBUILD_* variables.
# The arch Makefile must set $(boot-y) $(head-y), and extend the others.
ARCH_CPPFLAGS :=
ARCH_AFLAGS :=
ARCH_CFLAGS :=
include arch/$(SRCARCH)/Makefile

# include more CC flags
include scripts/Makefile.flags

# include extra warning CC flags
include scripts/Makefile.extrawarn

# The final assignments
KBUILD_CPPFLAGS	+= $(ARCH_CPPFLAGS) $(KCPPFLAGS)
KBUILD_AFLAGS	+= $(ARCH_AFLAGS)   $(KAFLAGS)
KBUILD_CFLAGS	+= $(ARCH_CFLAGS)   $(KCFLAGS)

# Default kernel image to build when no specific target is given.
# Usually arch Makefile would define its KBUILD_IMAGE. Also, it
# may be overruled on the command line or set in the environment.
export KBUILD_IMAGE ?= vmSandix

# INSTALL_PATH specifies where to place the updated kernel and system map
# images. Default is /boot, but you can set it to other values
export INSTALL_PATH ?= /boot

vmSandix-dirs	:= $(patsubst %/, %, $(filter %/, $(init-y) $(core-y) \
				$(libs-y) $(net-y) $(drivers-y)))
init-y		:= $(patsubst %/, %/built-in.o, $(init-y))
core-y		:= $(patsubst %/, %/built-in.o, $(core-y))
libs-y		:= $(patsubst %/, %/built-in.o, $(libs-y))
net-y		:= $(patsubst %/, %/built-in.o, $(net-y))
drivers-y	:= $(patsubst %/, %/built-in.o, $(drivers-y))

# Externally visible to link-vmSandix.sh
export KBUILD_VMSANDIX_INIT := $(head-y) $(init-y)
export KBUILD_VMSANDIX_MAIN := $(core-y) $(libs-y) $(net-y) $(drivers-y)
export KBUILD_VMSANDIX_LDS  := arch/$(SRCARCH)/kernel/vmSandix.ld

vmSandix-deps	:= $(KBUILD_VMSANDIX_INIT) $(KBUILD_VMSANDIX_MAIN) $(KBUILD_VMSANDIX_LDS)

quiet_cmd_link-vmSandix = LINK    $@
      cmd_link-vmSandix = $(CONFIG_SHELL) $< LD

# Final link of vmSandix
vmSandix: scripts/link-vmSandix.sh $(vmSandix-deps) FORCE
	$(call if_changed,link-vmSandix)

# The actual objects are built when descending,
# make sure no implicit rules kick in
$(sort $(vmSandix-deps)): $(vmSandix-dirs) ;

# Handle descending into subdirectories listed in $(vmSandix-dirs)
PHONY += $(vmSandix-dirs)
$(vmSandix-dirs): prepare scripts
	$(Q)$(MAKE) $(build)=$@

# Things we need to do before we recursively start building the kernel.
# Some are alreay listed in targets scripts or else, we list them here
# again to emphasize their importance in preparation.
PHONY += prepare
prepare: outputmakefile				\
	 archscripts				\
	 archprepare				\
	 archheaders				\
	 include/config/auto.conf		\
	 include/config/kernel.release		\
	 include/generated/version.h		\
	 include/generated/utsrelease.h		\
	 include/generated/compile.h FORCE

# Gnenerate some files before building vmSandix.
# UTS stands for: Unix Time Sharing.

define filechk_kernel.release
	echo "$(KERNELVERSION)"
endef

define filechk_version.h
	(echo \#define SANDIX_VERSION_CODE $(shell                         \
	expr $(VERSION) \* 65536 + 0$(PATCHLEVEL) \* 256 + 0$(SUBLEVEL)); \
	echo '#define KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))';)
endef

uts_len := 64
define filechk_utsrelease.h
	if [ `echo -n "$(KERNELRELEASE)" | wc -c ` -gt $(uts_len) ]; then \
	  echo '"$(KERNELRELEASE)" exceeds $(uts_len) characters' >&2;    \
	  exit 1;                                                         \
	fi;                                                               \
	(echo \#define UTS_RELEASE \"$(KERNELRELEASE)\";)
endef

include/config/kernel.release: include/config/auto.conf FORCE
	$(call filechk,kernel.release)

include/generated/version.h: $(srctree)/Makefile FORCE
	$(call filechk,version.h)

include/generated/utsrelease.h: include/config/kernel.release FORCE
	$(call filechk,utsrelease.h)

       chk_compile.h = :
 quiet_chk_compile.h = echo "$(COLOR_BEGIN)  CHK     $@$(COLOR_END)"
silent_chk_compile.h = :
include/generated/compile.h: Makefile FORCE
	@$($(quiet)chk_compile.h)
	$(Q)$(CONFIG_SHELL) $(srctree)/scripts/mkcompile_h $@ \
	"$(UTS_MACHINE)" "$(CONFIG_SMP)" "$(CONFIG_PREEMPT)" "$(CC) $(KBUILD_CFLAGS)"

# Clean
# ===========================================================================
# make distclean  - Clean all editor-tags and generated and compiled files
# make mrproper   - Clean all generated and compiled files
# make clean      - Clean all compiled files

quiet_cmd_rmdirs = $(if $(wildcard $(rm-dirs)),CLEAN   $(wildcard $(rm-dirs)))
      cmd_rmdirs = rm -rf $(rm-dirs)

quiet_cmd_rmfiles = $(if $(wildcard $(rm-files)),CLEAN   $(wildcard $(rm-files)))
      cmd_rmfiles = rm -f $(rm-files)

rm-dirs  := include/config include/generated
rm-files := .config .config.old .version .version.old scripts/.version tags tags-e

PHONY += distclean
distclean: mrproper
	@:

# Mr.Proper
PHONY += mrproper
mrproper: clean
	$(call cmd,rmdirs)
	$(call cmd,rmfiles)

# Add prefix to avoid overriding the previous targets.
clean-dirs := $(vmSandix-dirs) scripts/basic scripts/kconfig
clean-dirs := $(addprefix __clean__,$(clean-dirs))

PHONY += clean
clean: $(clean-dirs) archclean vmSandix-clean

# Clean files by descending into sub-directories
PHONY += $(clean-dirs)
$(clean-dirs):
	$(Q)$(MAKE) $(clean)=$(patsubst __clean__%,%,$@)

# Clean files generated by linking vmSandix
PHONY += vmSandix-clean
vmSandix-clean:
	$(Q)$(CONFIG_SHELL) $(srctree)/scripts/link-vmSandix.sh clean

# ===========================================================================
PHONY += help
help:
	@echo  "$(COLOR_BEGIN)"
	@echo  'Cleaning targets:'
	@echo  '  clean		  - Remove most generated files but keep the config'
	@echo  '  mrproper	  - Remove all generated files + config + various backup files'
	@echo  '  distclean	  - mrproper + remove editor backup'
	@echo  ''
	@echo  'Configuration targets:'
	@$(MAKE) -f $(srctree)/scripts/kconfig/Makefile help
	@echo  ''
	@echo  'Other generic targets:'
	@echo  '  all		  - Build all targets marked with [*]'
	@echo  '* vmSandix	  - Build the bare kernel'
	@echo  '  dir/            - Build all files in dir and below'
	@echo  '  dir/file.[oisS] - Build specified target only'
	@echo  '  tags/TAGS	  - Generate tags file for editors'
	@echo  '  kernelrelease	  - Output the release version string (use with make -s)'
	@echo  '  kernelversion	  - Output the version stored in Makefile (use with make -s)'
	@echo  '  image_name	  - Output the image name (use with make -s)'
	@echo  ''
	@echo  'Architecture specific targets ($(SRCARCH)):'
	@$(if $(archhelp),$(archhelp),\
		echo '  No architecture specific help defined for $(SRCARCH)')
	@echo  ''
	@$(if $(boards), \
		$(foreach b, $(boards), \
		printf "  %-24s - Build for %s\\n" $(b) $(subst _defconfig,,$(b));) \
		echo '')
	@$(if $(board-dirs), \
		$(foreach b, $(board-dirs), \
		printf "  %-16s - Show %s-specific targets\\n" help-$(b) $(b);) \
		printf "  %-16s - Show all of the above\\n" help-boards; \
		echo '')
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
	@echo  '  make M=n   [targets] Enable colorful output, n=0,1,2,3,4,5,6,7'
	@echo  '                0: black'
	@echo  '                1: red'
	@echo  '                2: green'
	@echo  '                3: yellow'
	@echo  '                4: blue'
	@echo  '                5: magenta'
	@echo  '                6: cyan'
	@echo  '                7: white'
	@echo  ''
	@echo  'Execute "make" or "make all" to build all targets marked with [*]'
	@echo  'For instructions on how to write Makefiles, please read Documentation/kbuild/*'
	@echo  "$(COLOR_END)"

# Generate tags for editors
# ===========================================================================
quiet_cmd_tags = GEN     $@
      cmd_tags = $(CONFIG_SHELL) $(srctree)/scripts/tags.sh $@

tags TAGS cscope gtags: FORCE
	$(call cmd,tags)

PHONY += kernelrelease
kernelrelease:
	@echo $(KERNELVERSION)

PHONY += kernelversion
kernelversion:
	@echo $(KERNELVERSION)

PHONY += image_name
image_name:
	@echo $(KBUILD_IMAGE)

PHONY += docs
docs:
	@echo 'docs'

# Single target
# ===========================================================================
# Single target - Most useful when you want to see, for example, the assembly
# code of C code, or the preprocessing file .i of C code. It is NOT intended
# for general users.
#
# Worth to mention, now it is your responsibility to clean generated files.
# I do NOT write rules for cleaning such rubbish.
#
# Single targets are compatible with:
# - build with mixed source and output
# - build with separate output dir 'make O=...'
#
#  target-dir => where to store outputfile
#  build-dir  => directory in kernel source tree to use

ifeq ($(KBUILD_EXTMOD),)
        build-dir  = $(patsubst %/,%,$(dir $@))
        target-dir = $(dir $@)
else
        zap-slash=$(filter-out .,$(patsubst %/,%,$(dir $@)))
        build-dir  = $(KBUILD_EXTMOD)$(if $(zap-slash),/$(zap-slash))
        target-dir = $(if $(KBUILD_EXTMOD),$(dir $<),$(dir $@))
endif

%.s: %.c prepare scripts FORCE
	$(Q)$(MAKE) $(build)=$(build-dir) $(target-dir)$(notdir $@)
%.i: %.c prepare scripts FORCE
	$(Q)$(MAKE) $(build)=$(build-dir) $(target-dir)$(notdir $@)
%.o: %.c prepare scripts FORCE
	$(Q)$(MAKE) $(build)=$(build-dir) $(target-dir)$(notdir $@)
%.s: %.S prepare scripts FORCE
	$(Q)$(MAKE) $(build)=$(build-dir) $(target-dir)$(notdir $@)
%.o: %.S prepare scripts FORCE
	$(Q)$(MAKE) $(build)=$(build-dir) $(target-dir)$(notdir $@)

endif # ! ifeq ($(config-targets),1)
endif # ! ifeq ($(mixed-targets),1)
endif # ! ifeq ($(skip-makefile),)

PHONY += FORCE
FORCE:

# Declare the contents of the .PHONY variable as phony.  We keep that
# information in a variable so we can use it in if_changed and friends.
.PHONY: $(PHONY)
