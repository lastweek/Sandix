#
#	arch/x86/Makefile - x86 Architecture
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

# Default .config for Kconfig
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
# x86 Architecture-Dependent Kernel Objects
# Little trick to separate arch-independent/dependent code.
#
head-y		:= arch/x86/kernel/head.o
core-y		+= arch/x86/kernel/
#libs-y		+= arch/x86/lib/
#core-y		+= arch/x86/mm/
#drivers-y	+= arch/x86/pci/

##
# Future: Bootloader support, like GRUB2
#
boot-y		:= arch/x86/boot
KBUILD_IMAGE	:= $(boot-y)/bzImage

##
# BUILD the x86 bzImage:
#
all: bzImage
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
