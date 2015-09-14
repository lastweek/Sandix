#
#	arch/x86/Makefile - Build x86 Architecture
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

ifneq ($(ARCH),x86)
  $(error arch/x86/Makefile is included improperly)
endif

ifneq ($(CONFIG_X86_32),y)
  $(error CONFIG_X86_32 is not set)
endif

##
# Default .config prepared for Kconfig
#
KBUILD_DEFCONFIG := i386_defconfig

##
# How to compile the 16-bit code.  Note we always compile for -march=i386;
# that way we can complain to the user if the CPU is insufficient.
# The -m16 option is supported by GCC>=4.9. For older versions of GCC, include
# an assembly header to make sure GCC does not play any games behind our back.
#
CODE16GCC_CFLAGS := -m32 -Wa,$(srctree)/arch/x86/boot/code16gcc.h
M16_CFLAGS	 := $(call cc-option, -m16, $(CODE16GCC_CFLAGS))

REALMODE_CFLAGS  := $(M16_CFLAGS) -Os -D__KERNEL__ \
		   -Wall -Wstrict-prototypes -march=i386 -mregparm=3 \
		   -fno-strict-aliasing -fomit-frame-pointer -fno-pic \
		   -mno-mmx -mno-sse \
		   $(call cc-option, -ffreestanding) \
		   $(call cc-option, -fno-stack-protector) \
		   $(call cc-option, -mpreferred-stack-boundary=2)
export REALMODE_CFLAGS

##
# FLAGS for 32-bit Kernel
#

CHECKFLAGS	+= -D__i386__
LDFLAGS		+= -m elf_i386

BINARCH		:= $(call cc-option,-m32)
KBUILD_CFLAGS	+= $(BINARCH)
KBUILD_AFLAGS	+= $(BINARCH)

# Freestanding
KBUILD_CFLAGS	+= -ffreestanding
       
# Use registers to pass parameters
KBUILD_CFLAGS	+= -mregparm=3

# Return struct, union in registers when possible
KBUILD_CFLAGS	+= -freg-struct-return

# Never want PIC in 32-bit kernel
KBUILD_CFLAGS	+= -fno-pic

# Prevent GCC from keeping stack 16 bytes aligned
KBUILD_CFLAGS	+= $(call cc-opinon,-mpreferred-stack-boundary=2)

# Speed up the build
KBUILD_CFLAGS	+= -pipe

# Prevent GCC from generating any FP code by mistake
KBUILD_CFLAGS	+= -mno-sse -mno-mmx -mno-sse2 -mno-3dnow

##
# x86 Architecture-Dependent Kernel Built-in
#
# Note that: We separate kernel into different parts,
# $(head-y), $(init-y), $(core-y) and so on. Normally,
# the $(head-y) is head.o only, used to jump to $(init-y)
#
head-y		:= arch/x86/kernel/head.o
core-y		+= arch/x86/kernel/
libs-y		+= arch/x86/lib/
#core-y		+= arch/x86/mm/
#drivers-y	+= arch/x86/pci/

##
# x86 Architecture-Dependent Kernel Setup
#
# Note that: In x86, the setup part is under real-mode.
# The setup part of x86 needs to communicate with bootloader
# such as GRUB or LILO.
#
boot-y		:= arch/x86/boot
KBUILD_IMAGE	:= $(boot-y)/bzImage

all: bzImage

##
# A phony bzImage. The real bzImage is built in $(boot-y)
#
PHONY += bzImage
bzImage:
	$(Q)$(MAKE) $(build)=$(boot-y) $(KBUILD_IMAGE)

##
# Architecture Specific Clean
#
PHONY += arch-clean
arch-clean:
	$(Q)$(MAKE) $(clean)=$(boot-y)

##
# Architecture Specific Help
#
PHONY += arch-help
arch-help:
	@echo  'x86 HELP'
