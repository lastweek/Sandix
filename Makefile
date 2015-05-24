# Base on Linux Makefile ;)
VERSION = 1
PATCHLEVEL = 0
SUBLEVEL = 0
NAME0 = Sandix
NAME1 = Crape myrtle
NAME2 = Lagerstroemia indica

# ===========================================================================
# Beautify output
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

export quiet Q KBUILD_VERBOSE


# ===========================================================================
# Nowadays Sandix don't support building in other directories.
# But we use these two variables for future development.

srctree = .
objtree = .
export srctree objtree

# o Do not use make's built-in rules and variables,
#   this increases performance and avoids hard-to-debug behaviour.
# o Look for make include files relative to root of kernel src
MAKEFLAGS += -rR --include-dir=$(srctree)
#MAKEFLAGS += --print-directory

# FIXME i386-elf-gcc toolchains in my macos.
CROSS_COMPILE = i386-elf-

# ===========================================================================
# Make variables (CC, etc...)
CC		= $(CROSS_COMPILE)gcc
AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CPP		= $(CC) -E
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm
STRIP	= $(CROSS_COMPILE)strip
OBJCOPY	= $(CROSS_COMPILE)objcopy
OBJDUMP	= $(CROSS_COMPILE)objdump
MAKE	= make
AWK 	= awk
PERL	= perl
PYTHON	= python

KBUILD_CFLAGS := \
			-Wall -Wundef -Wstrict-prototypes -Wno-trigraphs \
			-fno-strict-aliasing -fno-common -ffreestanding \
			-Werror-implicit-function-declaration \
			-Wno-format-security \
			-std=gnu89

KBUILD_CPPFLAGS	:= -D__KERNEL__
KBUILD_LDFLAGS	:= 
KBUILD_AFLAGS	:= -D__ASSEMBLY__
SANDIXINCLUDE	:= -I$(srctree)/include/
NOSTDINC_FLAGS	:= -nostdinc

export VERSION PATCHLEVEL SUBLEVEL NAME0 NAME1 NAME2
export CC AS LD CPP AR NM STRIP OBJCOPY OBJDUMP
export MAKE AWK PERL PYTHON
export KBUILD_CFLAGS KBUILD_CPPFLAGS KBUILD_LDFLAGS KBUILD_AFLAGS
export SANDIXINCLUDE NOSTDINC_FLAGS


# ---------------------------------------------------------------------------
#* Target: all
#* Depend on: vmsandix
#* Desc: Useless
# ---------------------------------------------------------------------------
PHONY := all
all: vmsandix

# Some generic definitions
include $(srctree)/scripts/Kbuild.include

# Dependencies of vmsandix
boot-y			:= boot/
#init-y			:= init/
#core-y			:= kernel/ mm/ fs/ ipc/ block/
#drivers-y		:= drivers/

vmsandix-dirs	:= $(patsubst %/, %, $(boot-y) $(init-y) $(core-y) $(drivers-y))

boot-y			:= $(patsubst %/, %/built-in.o, $(boot-y))
init-y			:= $(patsubst %/, %/built-in.o, $(init-y))
core-y			:= $(patsubst %/, %/built-in.o, $(core-y))
drivers-y		:= $(patsubst %/, %/built-in.o, $(drivers-y))

KBUILD_VMSANDIX_INIT := $(boot-y) $(init-y)
KBUILD_VMSANDIX_MAIN := $(core-y) $(drivers-y)

vmsandix-deps := $(KBUILD_VMSANDIX_INIT) $(KBUILD_VMSANDIX_MAIN)

export KBUILD_VMSANDIX_INIT KBUILD_VMSANDIX_MAIN

# ---------------------------------------------------------------------------
#* Target: vmsandix
#* Depend on: $(vmsandix-deps), like fs/built-in.o init/built-in.o ...
#* Desc: Build Sandix kernel image.
# ---------------------------------------------------------------------------
PHONY += vmsandix
vmsandix: $(vmsandix-deps)

# ---------------------------------------------------------------------------
#* Target: $(vmsandix-deps)
#* Depend on: $(vmsandix-dir)
#* Desc: Descending into $(vmsandix-dir) to BUILD built-in.o
# ---------------------------------------------------------------------------
# Still doubt doning this ;)
$(sort $(vmsandix-deps)): $(vmsandix-dirs) ;
PHONY += $(vmsandix-dirs)
$(vmsandix-dirs):
	$(Q)$(MAKE) $(build)=$@

# ---------------------------------------------------------------------------
#* Target: help
#* Depend on:
#* Desc: Print help information about Makefile usage.
# ---------------------------------------------------------------------------
PHONY += help
help:
	@echo "Top Makefile of Sandix Kernel"
	@echo "PHONYS: $(PHONY)"

# ---------------------------------------------------------------------------
#* Target: clean
#* Depend on:
#* Desc: Clean generated files during building process.
# ---------------------------------------------------------------------------
PHONY += clean
clean:
	@echo "Do nothing now..."

# ---------------------------------------------------------------------------
#* Target: FORCE
#* Depend on:
#* Desc: Add FORCE to force a target to be always rebuilt.
# ---------------------------------------------------------------------------
PHONY += FORCE
FORCE:

# ---------------------------------------------------------------------------
# There are two reasons to use a phony target:
# 1) to avoid a conflict with a file of the same name,
# 2) to improve performance.
# ---------------------------------------------------------------------------
.PHONY: $(PHONY)
