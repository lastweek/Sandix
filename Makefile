VERSION = 1
PATCHLEVEL = 0
SUBLEVEL = 0
NAME0 = Sandix
NAME1 = Crape myrtle
NAME2 = Lagerstroemia indica

# ===========================================================================
# Nowadays Sandix don't support building in other directories.
# But we use these two variables for future development.
srctree = .
objtree = .
export srctree objtree

# Do not use make's built-in rules and variables,
# this increases performance and avoids hard-to-debug behaviour.
MAKEFLAGS += -rR

# Look for make include files relative to root of kernel src
MAKEFLAGS += --include-dir=$(srctree)

# Noisy, but so yummy for me.
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
			-fno-strict-aliasing -fno-common \
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


# ===========================================================================
PHONY := all
all: vmsandix

# Some generic definitions
include $(srctree)/scripts/Kbuild.include

# Dependencies of vmsandix
boot-y			:= boot/
init-y			:= init/
core-y			:= kernel/ mm/ fs/ ipc/ block/
drivers-y		:= drivers/

vmsandix-dirs	:= $(patsubst %/, %, $(boot-y) $(init-y) $(core-y) $(drivers-y))
boot-y			:= $(patsubst %/, %/built-in.o, $(boot-y))
init-y			:= $(patsubst %/, %/built-in.o, $(init-y))
core-y			:= $(patsubst %/, %/built-in.o, $(core-y))
drivers-y		:= $(patsubst %/, %/built-in.o, $(drivers-y))

KBUILD_VMSANDIX_INIT := $(boot-y) $(init-y)
KBUILD_VMSANDIX_MAIN := $(core-y) $(drivers-y)
vmsandix-deps := $(KBUILD_VMSANDIX_INIT) $(KBUILD_VMSANDIX_MAIN)
export KBUILD_VMSANDIX_INIT KBUILD_VMSANDIX_MAIN


# ===========================================================================
# vmsandix's dependencies are built-in.o files in each directory.
PHONY += vmsandix
vmsandix: $(vmsandix-deps)

# ===========================================================================
# The actual objects are generated when descending,
# make sure no implicit rule kicks in
$(sort $(vmsandix-deps)): $(vmsandix-dirs) ;

# ===========================================================================
# Dsecending to each directory.
PHONY += $(vmsandix-dirs)
$(vmsandix-dirs):
	$(MAKE) $(build)=$@

# ===========================================================================
PHONY += help
help:
	@echo "Top Makefile of Sandix Kernel"
	@echo "PHONYS: $(PHONY)"

# ===========================================================================
PHONY += clean
clean:
	@rm 

# ===========================================================================
PHONY += FORCE
FORCE:

# There are two reasons to use a phony target:
# to avoid a conflict with a file of the same name,
# and to improve performance.
.PHONY: $(PHONY)
