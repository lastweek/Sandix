#
#	arch/x86/boot/Makefile - Build the final bzImage
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

hostprogs-y := tools/build

obj-y	:= a20.o


##
# Safe to override KBUILD_CFLAGS, KBUILD_AFLAGS,
# since this is the last step of our build.
#
KBUILD_CFLAGS	:= $(USERINCLUDE) $(REALMODE_CFLAGS) -D__SETUP__
KBUILD_AFLAGS	:= $(KBUILD_CFLAGS) -D__ASSEMBLY_

##
# BUILD THE FINAL arch/x86/boot/bzImage
# bzImage need four things:
#	tools/build	-	Tool to concatenate
#	bootsect.bin	-	The so-called bootloader
#	setup.bin	-	The real-mode kernel image
#	vmSandix.bin	-	The bare kernel itself
#
quiet_cmd_image = BUILD   $@
      cmd_image = $(obj)/tools/build
$(obj)/bzImage: $(obj)/setup.bin $(obj)/vmSandix.bin $(obj)/tools/build FORCE
	$(call if_changed,image)
	@echo 'Sandix Kernel: $@ is ready'
##
# Convert vmSandix to raw binary file
#
OBJCOPYFLAGS_vmSandix.bin := -O binary -R .note -R .comment -S
$(obj)/vmSandix.bin: $(obj)/vmSandix FORCE
	$(call if_changed,objcopy)

##
# Convert setup to raw binary file
#
OBJCOPYFLAGS_setup.bin := -O binary
$(obj)/setup.bin: $(obj)/setup FORCE
	$(call if_changed,objcopy)

