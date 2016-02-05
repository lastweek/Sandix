#	Copyright (C) 2015-2016 Yizhou Shan <shanyizhou@ict.ac.cn>
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

ifneq ($(SRCARCH),x86)
  $(error arch/x86/Makefile is included improperly)
endif

ifeq ($(CONFIG_X86_32),y)
KBUILD_DEFCONFIG := i386_defconfig
else
KBUILD_DEFCONFIG := x86_64_defconfig
endif

# Flags
# ===========================================================================

# How to compile the 16-bit code.  Note we always compile for -march=i386;
# that way we can complain to the user if the CPU is insufficient.
#
# The -m16 option is supported by GCC>=4.9. For older versions of GCC, include
# an *assembly* header to make sure GCC does not play any games behind our back.
CODE16GCC_CFLAGS := -m32 -Wa,$(srctree)/arch/x86/boot/code16gcc.h
M16_CFLAGS	 := $(call cc-option, -m16, $(CODE16GCC_CFLAGS))

REALMODE_CFLAGS  := $(M16_CFLAGS) -Os -D__KERNEL__			\
		   -Wall -Wstrict-prototypes -march=i386 -mregparm=3	\
		   -fno-strict-aliasing -fomit-frame-pointer -fno-pic	\
		   -mno-mmx -mno-sse					\
		   $(call cc-option, -ffreestanding)			\
		   $(call cc-option, -fno-stack-protector)		\
		   $(call cc-option, -mpreferred-stack-boundary=2)

export REALMODE_CFLAGS

# BITS is used as extension for files which are available in a 32 bit
# and a 64 bit version to simplify shared Makefiles.
# e.g.: obj-y += foo_$(BITS).o
export BITS

# Prevent GCC from generating any FP code by mistake.
#
# This must happen before we try the -mpreferred-stack-boundary, see:
#
#    https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53383
#
KBUILD_CFLAGS += -mno-sse -mno-mmx -mno-sse2 -mno-3dnow
KBUILD_CFLAGS += $(call cc-option,-mno-avx,)

ifeq ($(CONFIG_X86_32),y)
        BITS		:= 32
        UTS_MACHINE	:= i386
        CHECKFLAGS	+= -D__i386__

        binarch		:= $(call cc-option,-m32)
        KBUILD_CFLAGS	+= $(binarch)
        KBUILD_AFLAGS	+= $(binarch)

        KBUILD_CFLAGS	+= -msoft-float

        # Use registers to pass parameters
        KBUILD_CFLAGS	+= -mregparm=3

        # Return struct, union in registers when possible
        KBUILD_CFLAGS	+= -freg-struct-return

        # Never want PIC in a 32-bit kernel, prevent breakage with GCC built
        # with nonstandard options
        KBUILD_CFLAGS += -fno-pic

        # Prevent GCC from keeping stack 16 bytes aligned
        KBUILD_CFLAGS	+= $(call cc-opinon,-mpreferred-stack-boundary=2)

        # Disable unit-at-a-time mode on pre-gcc-4.0 compilers, it makes gcc use
        # a lot more stack due to the lack of sharing of stacklots:
        KBUILD_CFLAGS += $(call cc-ifversion, -lt, 0400, \
				$(call cc-option,-fno-unit-at-a-time))

        # temporary until string.h fixed
        KBUILD_CFLAGS	+= -ffreestanding
else
        BITS := 64
        UTS_MACHINE := x86_64
        CHECKFLAGS += -D__x86_64__ -m64

        biarch := -m64
        KBUILD_AFLAGS += -m64
        KBUILD_CFLAGS += -m64

        # Align jump targets to 1 byte, not the default 16 bytes:
        KBUILD_CFLAGS += -falign-jumps=1

        # Pack loops tightly as well:
        KBUILD_CFLAGS += -falign-loops=1

        # Don't autogenerate traditional x87 instructions
        KBUILD_CFLAGS += $(call cc-option,-mno-80387)
        KBUILD_CFLAGS += $(call cc-option,-mno-fp-ret-in-387)

	# Use -mpreferred-stack-boundary=3 if supported.
        KBUILD_CFLAGS += $(call cc-option,-mpreferred-stack-boundary=3)

	# Use -mskip-rax-setup if supported.
        KBUILD_CFLAGS += $(call cc-option,-mskip-rax-setup)
     
        KBUILD_CFLAGS += -mno-red-zone
        KBUILD_CFLAGS += -mcmodel=kernel

        # -funit-at-a-time shrinks the kernel .text considerably
        # unfortunately it makes reading oopses harder.
        KBUILD_CFLAGS += $(call cc-option,-funit-at-a-time)

        # this works around some issues with generating unwind tables in older gccs
        # newer gccs do it by default
        KBUILD_CFLAGS += $(call cc-option,-maccumulate-outgoing-args)
endif

LDFLAGS := -m elf_$(UTS_MACHINE)

# Speed up the build
KBUILD_CFLAGS += -pipe

# Warn when a comparison between signed and unsigned values could produce
# an incorrect result when the signed value is converted to unsigned
#KBUILD_CFLAGS += Wno-sign-compare

# ?
KBUILD_CFLAGS += -fno-asynchronous-unwind-tables

# General Targets
# ===========================================================================

archprepare:
	@:

archscripts:
	@:

# Syscall table generation
archheaders:
	@:

archclean:
	$(Q)rm -rf $(objtree)/arch/i386
	$(Q)rm -rf $(objtree)/arch/x86_64
	$(Q)$(MAKE) $(clean)=$(boot-y)

define archhelp
  echo  '* bzImage      - Compressed kernel image (arch/x86/boot/bzImage)'
  echo  '  install      - Install kernel to $(INSTALL_PATH)'
endef

# Kernel objects
# ===========================================================================
# Here we init or extend the kernel objects predefined in top Makefile.
# All directories listed below are architecture-dependent. Especially the
# $(boot-y) part, this is real-mode 16-bit code.

boot-y		:= arch/x86/boot
KBUILD_IMAGE	:= $(boot-y)/bzImage

head-y		:= arch/x86/kernel/head_$(BITS).o

# List entry after kernel, see kernel/Makefile comment
core-y		+= arch/x86/kernel/
core-y		+= arch/x86/entry/
core-y		+= arch/x86/mm/

libs-y		+= arch/x86/lib/

drivers-$(CONFIG_PCI) += arch/x86/pci/

# The all: target is the default when no target is given on the command line.
# This allow a user to issue only 'make' to build Sandix kernel.
# In x86 building, the final kernel image is bzImage
all: bzImage

# This is the phony one (the pathname...). The real one is in boot
PHONY += bzImage
bzImage: vmSandix
	$(Q)$(MAKE) $(build)=$(boot-y) $(KBUILD_IMAGE)
	$(Q)mkdir -p $(objtree)/arch/$(UTS_MACHINE)/boot
	$(Q)ln -fsn ../../x86/boot/bzImage $(objtree)/arch/$(UTS_MACHINE)/boot/$@

# Install bzImage and System.map
PHONY += install
install:
	$(Q)$(MAKE) $(build)=$(boot-y) $@
