VERSION = 1
PATCHLEVEL = 0
SUBLEVEL = 0
NAME0 = Sandix
NAME1 = Crape myrtle
NAME2 = Lagerstroemia indica

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

export quiet Q KBUILD_VERBOSE


# Sandix does not support building in other directories.
srctree = .
objtree = .
export srctree objtree

# o Do not use make's built-in rules and variables,
#   this increases performance and avoids hard-to-debug behaviour.
# o Look for make include files relative to root of kernel src
MAKEFLAGS += -rR --include-dir=$(srctree)

# FIXME i386-elf-gcc toolchains in my macos.
CROSS_COMPILE = i386-elf-

# VARIABLES
# ===========================================================================
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

KBUILD_CFLAGS := -std=gnu89 -pipe -Wall -Wundef
KBUILD_CFLAGS += -fno-strict-aliasing -fno-common
KBUILD_CFLAGS += -Wno-format-security
KBUILD_CFLAGS += -Wdeclaration-after-statement
KBUILD_CFLAGS += -Werror=strict-prototypes
KBUILD_CFLAGS += -Werror=implicit-function-declaration
#KBUILD_CFLAGS += -O2

KBUILD_CPPFLAGS	:= -D__KERNEL__
KBUILD_AFLAGS	:= -D__ASSEMBLY__
KBUILD_LDFLAGS	:=

SANDIXINCLUDE	:= -I$(srctree)/include/
NOSTDINC_FLAGS	:= -nostdinc

OBJCOPYFLAGS	:= -j .text -j .text32 -j .data -j .rodata -O binary
OBJDUMPFLAGS	:= -d -M att

export VERSION PATCHLEVEL SUBLEVEL NAME0 NAME1 NAME2
export CC AS LD CPP AR NM STRIP OBJCOPY OBJDUMP
export MAKE AWK PERL PYTHON
export KBUILD_CFLAGS KBUILD_CPPFLAGS KBUILD_LDFLAGS KBUILD_AFLAGS
export SANDIXINCLUDE NOSTDINC_FLAGS
export OBJCOPYFLAGS OBJDUMPFLAGS

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

	# No builtin-function?
    KBUILD_CFLAGS += -ffreestanding
endif

boot-y			:= boot/
init-y			:= init/
core-y			:= mm/ lib/
drivers-y		:= drivers/
#core-y			:= kernel/ mm/ fs/ ipc/ block/
vmsandix-dirs	:= $(patsubst %/, %, $(boot-y) $(init-y) $(core-y) $(drivers-y))

boot-y			:= $(patsubst %/, %/built-in.o, $(boot-y))
init-y			:= $(patsubst %/, %/built-in.o, $(init-y))
core-y			:= $(patsubst %/, %/built-in.o, $(core-y))
drivers-y		:= $(patsubst %/, %/built-in.o, $(drivers-y))
vmsandix-deps	:= $(boot-y) $(init-y) $(core-y) $(drivers-y)

KBUILD_VMSANDIX_BOOT := $(boot-y)
KBUILD_VMSANDIX_MAIN := $(init-y) $(core-y) $(drivers-y)

export KBUILD_VMSANDIX_BOOT KBUILD_VMSANDIX_MAIN

# BIT FAT NOTE:
# o Real-Mode kernel image is boot/rmimage.
# o Link $(init-y) $(core-y) $(drivers-y) together to form protected-mode
#   kernel image. Move and rename the pm kernel image to boot/pmimage.
# o boot/CATENATE is used to catenate bootloader, rmimage and
#   pmimage together to form bzImage.
_RM_IMAGE := boot/rmimage
_PM_IMAGE := boot/pmimage
RM_IMAGE  := boot/rmimage.bin
PM_IMAGE  := boot/pmimage.bin
VMSANDIX  := boot/vmsandix
BZIMAGE   := boot/bzImage
RM_LD_CMD := scripts/rm-image.ld
PM_LD_CMD := scripts/pm-image.ld

# Some generic definitions and variables
include $(srctree)/scripts/Kbuild.include


# COMMANDS FOR BZIMAGE
# ===========================================================================
quiet_cmd_link_rm := LD $(SS)  $(_RM_IMAGE)
      cmd_link_rm := $(LD) -T $(RM_LD_CMD) -o $(_RM_IMAGE) $(KBUILD_VMSANDIX_BOOT)

quiet_cmd_link_pm := LD $(SS)  $(_PM_IMAGE)
      cmd_link_pm := $(LD) -T $(PM_LD_CMD) -o $(_PM_IMAGE) $(KBUILD_VMSANDIX_MAIN)

quiet_cmd_bin_rm := OBJCOPY $(RM_IMAGE)
      cmd_bin_rm := $(OBJCOPY) $(OBJCOPYFLAGS) $(_RM_IMAGE) $(RM_IMAGE)

quiet_cmd_bin_pm := OBJCOPY $(PM_IMAGE)
      cmd_bin_pm := $(OBJCOPY) $(OBJCOPYFLAGS) $(_PM_IMAGE) $(PM_IMAGE)

quiet_cmd_catenate := CAT$(SS) $(VMSANDIX)
      cmd_catenate := ./boot/CATENATE


# BUILD
# ===========================================================================
PHONY := all
all: bzImage vmsandix

bzImage: vmsandix
	@chmod +x $(VMSANDIX)
	@mv $(VMSANDIX) $(BZIMAGE)

# FIXME boot/ should be separated from deps
vmsandix: $(vmsandix-deps)
	$(call if_changed,link_pm)
	$(call if_changed,link_rm)
	$(call if_changed,bin_pm)
	$(call if_changed,bin_rm)
	$(call if_changed,catenate)

$(sort $(vmsandix-deps)): $(vmsandix-dirs) ;

# Descending into sub-directory
PHONY += $(vmsandix-dirs)
$(vmsandix-dirs):
	$(Q)$(MAKE) $(BUILD)=$@


# CLEAN
# ===========================================================================

# Add prefix to avoid overriding the previous targets.
CLEAN_DIRS := $(addprefix __CLEAN__,$(vmsandix-dirs))

PHONY += clean
clean: $(CLEAN_DIRS)
	@rm -f $(_RM_IMAGE) $(RM_IMAGE)
	@rm -f $(_PM_IMAGE) $(PM_IMAGE)
	@rm -f $(VMSANDIX) $(BZIMAGE)

$(CLEAN_DIRS):
	$(Q)$(MAKE) $(CLEAN)=$(patsubst __CLEAN__%,%,$@)


# HELP
# ===========================================================================
PHONY += help
help:
	@echo "Build Sandix Kernel"

PHONY += FORCE
FORCE:

# There are two reasons to use a phony target:
# 1) to avoid a conflict with a file of the same name,
# 2) to improve performance.
.PHONY: $(PHONY)
