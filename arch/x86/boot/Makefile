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

##
# Safe to override KBUILD_CFLAGS, KBUILD_AFLAGS,
# since this is the last step of kbuild.
#
KBUILD_CFLAGS	:= $(USERINCLUDE) $(REALMODE_CFLAGS) -D__SETUP__
KBUILD_AFLAGS	:= $(KBUILD_CFLAGS) -D__ASSEMBLY__

##
# HOST BUILD
#
hostprogs-y := tools/build

##
# Simple Boot Loader
# TO BE REPLACED. Sandix should work with GRUB or something.
#
loader-y := bootloader.o

##
# Since Makefile.build has no rules for these
# targets, so we have to add them manually. Then
# they can get their saved-old commands included,
# then they can use $(if_changed_cmd) freely.
# Also, Makefile.clean knows how to clean them too.
#
targets += voffset.h
targets += vmSandix.bin
targets += bootloader.bin
targets += setup.elf
targets += setup.bin
targets += bzImage

##
# Kernel Setup Part. (Real-Mode in x86, actually)
# header.o must come first, as its name.
#
setup-y	:= header.o
setup-y	+= main.o
setup-y	+= a20.o
setup-y	+= bioscall.o
setup-y	+= memory.o
setup-y	+= pm.o
setup-y	+= pmjump.o 
setup-y	+= printf.o
setup-y	+= regs.o
setup-y	+= string.o
setup-y	+= tty.o
setup-y += version.o
setup-y	+= video.o
setup-targets := $(addprefix $(obj)/, $(setup-y))

##
# Building bzImage need four things:
#	bootloader.bin	-	The so-called bootloader
#	setup.bin	-	The real-mode kernel image
#	vmSandix.bin	-	The bare kernel itself
#	tools/build	-	Tool to concatenate
#	voffset.h	-	Kernel virtual address offset
#
quiet_cmd_build_image = BUILD   $@
      cmd_build_image = $(obj)/tools/build $(obj)/bootloader.bin	\
      			$(obj)/setup.bin $(obj)/vmSandix.bin $@

$(obj)/bzImage: $(obj)/bootloader.bin $(obj)/setup.bin $(obj)/voffset.h \
                $(obj)/vmSandix.bin $(obj)/tools/build FORCE
	$(call if_changed,build_image)
	@chmod +x $(obj)/bzImage
	@echo 'Sandix Kernel: $@ is ready.'

sed-voffset := -e 's/^\([0-9a-fA-F]*\) [ABCDGRSTVW] \(kstart\|kend\)/\#define VO_\2 0x\1/p'
quiet_cmd_voffset = VOFFSET $@
      cmd_voffset = $(NM) $< | sed -n $(sed-voffset) > $@

$(obj)/voffset.h: $(obj)/vmSandix FORCE
	$(call if_changed,voffset)

AFLAGS_header.o := -I$(obj)
$(obj)/header.o: $(obj)/voffset.h

OBJCOPYFLAGS_vmSandix.bin := -O binary -R .note -R .comment -S
$(obj)/vmSandix.bin: $(obj)/vmSandix FORCE
	$(call if_changed,objcopy)

OBJCOPYFLAGS_bootloader.bin := -O binary -R .note -R .comment -S
$(obj)/bootloader.bin: $(obj)/bootloader.o FORCE
	$(call if_changed,objcopy)

OBJCOPYFLAGS_setup.bin := -O binary -R .note -R .comment -S
$(obj)/setup.bin: $(obj)/setup.elf FORCE
	$(call if_changed,objcopy)

LDFLAGS_setup.elf := -T
$(obj)/setup.elf: $(src)/setup.ld $(setup-targets) FORCE
	$(call if_changed,ld)
