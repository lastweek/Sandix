VERSION = 1
PATCHLEVEL = 0
SUBLEVEL = 0
NAME0 = Sandix
NAME1 = Crape myrtle
NAME2 = Lagerstroemia indica

# Nowadays sandix don't support building in other directories.
# But we use $(srctree) for future update!
srctree = .

# SHELL used by build.
CONFIG_SHELL := $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
      else if [ -x /bin/bash ]; then echo /bin/bash; \
	  else echo sh; fi ; fi)

# Because i am using i386-elf-gcc toolchains in my macos.
CROSS_COMPILE = i386-elf-

# 1. Do not use make's built-in rules and variables
# 	(this increases performance and avoids hard-to-debug behaviour);
# 2. Look for make include files relative to root of kernel src
MAKEFLAGS += -rR
MAKEFLAGS += --include-dir=$(srctree)
MAKEFLAGS += --print-directory


#############################
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
MAKE    = make
AWK		= awk
PERL	= perl
PYTHON	= python

KBUILD_CFLAGS   := -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs \
		   -fno-strict-aliasing -fno-common \
		   -Werror-implicit-function-declaration \
		   -Wno-format-security \
		   -std=gnu89

export VERSION PATCHLEVEL SUBLEVEL NAME0 NAME1 NAME2
export CC AS LD CPP AR NM STRIP OBJCOPY OBJDUMP
export MAKE AWK PERL PYTHON
export KBUILD_CFLAGS

# We need some generic definitions (do not try to remake the file).
#$(srctree)/scripts/Kbuild.include: ;
include $(srctree)/scripts/Kbuild.include


#############################
# The default make target
all: zsandix
	@echo $(CONFIG_SHELL)


zsandix:
	@echo "zsandix"

help:
	@echo "Need to do"

clean:




