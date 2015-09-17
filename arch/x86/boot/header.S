#
#	arch/x86/boot/header.S - The real head of Sandix kernel.
#	Fit with linux x86 boot protocol, can work with BootLoader
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

#include <asm/boot.h>
#include <asm/bootparam.h>

BOOTSEG		= 0x07C0	# Historically boot sect address
SYSSEG		= 0x1000	# Kernel System load address >> 4
SETUPSEG	= 0x9000	# Kernel Setup load address >> 4

#ifndef ROOT_RDONLY
# define ROOT_RDONLY	1
#endif

#ifndef STACK_SIZE
# define STACK_SIZE	512
#endif

##
# Sandix should be loaded by boot loader. If no loader is
# avaliable, the bootsect is loaded by BIOS directly into
# 0x7c00. If that happens, we do nothing but print some
# message to tell the user that a boot loader is needed.
#

	.code16

#------------------------------------------------------------
# SECTOR 0, OFFSET 0, 0x9000:0000
	.section ".bstext", "ax"
	.global boosect_start
bootsect_start:
	# Normalize the start address
	ljmp	$BOOTSEG, $start2
start2:
	movw	%cs, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	xorw	%sp, %sp
	sti
	cld
	movw	$bootsect_msg, %si
msg_loop:
	lodsb
	andb	%al, %al
	jz	bootsect_die
	movb	$0xe, %ah
	movw	$7, %bx
	int	$0x10
	jmp	msg_loop
bootsect_die:
	# Allow the user to press a key, then reboot
	xorw	%ax, %ax
	int	$0x16
	int	$0x19
	
	# Int 0x19 never return. In case it does
	# anyway, invoke the BIOS reset code.
	ljmp	$0xf000,$0xfff0

	.section ".bsdata", "a"
bootsect_msg:
	.ascii "Please use a boot loader.\r\n"
	.ascii "\n"
	.ascii "Remove disk and press any key to reboot..."
	.byte  0

##
# Kernel attributes. Used by setup.
# This is part 1 of the header, for the old boot sector.
#

##
# Start of setup header ###################################################
#

#------------------------------------------------------------
# SECTOR 0, OFFSET 495, 0x9000:1ef
	.section ".header", "a"
sentinel:	.byte 0xff, 0xff	# Used to detect broken loaders

	.globl	hdr
hdr:
setup_sects:	.byte 0			# Filled in by build.c
root_flags:	.word ROOT_RDONLY
syssize:	.long 0			# Filled in by build.c
ram_size:	.word 0			# Obsolete
vid_mode:	.word NORMAL_VGA	# DO NOT NEED TO ASK
root_dev:	.word 0			# Filled in by build.c
boot_flag:	.word 0xAA55

#------------------------------------------------------------
# SECTOR 1, OFFSET 0, 0x9000:0200
	.global _start
_start:
	# Explicitly enter this as bytes, or the assembler
	# tries to generate a 3-byte jump here, which causes
	# everything else to push off to the wrong offset.
	.byte	0xeb	# short (2-byte) jump
	.byte	start_of_setup-1f
1:

##
# Part 2 of the header
#
	.ascii	"HdrS"					# header signature
	.word	0x020d					# header version number (>= 0x0105)
							# or else old loadlin-1.5 will fail)
	.globl realmode_swtch
realmode_swtch:	 .word	0, 0				# default_switch, SETUPSEG
start_sys_seg:	 .word	SYSSEG				# obsolete and meaningless, but just
							# in case something decided to "use" it
		 #.word	kernel_version-512 		# pointing to kernel version string
		 .word	0				# pointing to kernel version string
							# above section of header is compatible
							# with loadlin-1.5 (header v1.5). Don't
							# change it.

type_of_loader:	 .byte	0				# 0 means ancient bootloader, newer
							# bootloaders know to change this.
							# See Documentation/x86/boot.txt for
							# assigned ids

loadflags:	 .byte	LOADED_HIGH			# The kernel is to be loaded high
							# Unused bits must be zero bit

setup_move_size: .word  0x8000				# size to move, when setup is not
							# loaded at 0x90000. We will move setup
							# to 0x90000 then just before jumping
							# into the kernel. However, only the
							# loader knows how much data behind
							# us also needs to be loaded.

code32_start:						# here loaders can put a different
							# start address for 32-bit code.
		.long	0x100000			# 0x100000 = default for big kernel

ramdisk_image:	.long	0				# address of loaded ramdisk image
							# Here the loader puts the 32-bit
							# address where it loaded the image.
							# This only will be read by the kernel.

ramdisk_size:	.long	0				# its size in bytes

bootsect_kludge:
		.long	0				# obsolete

heap_end_ptr:	.word	__end+STACK_SIZE-512
							# (Header version 0x0201 or later)
							# space from here (exclusive) down to
							# end of setup code can be used by setup
							# for local heap purposes.
ext_loader_ver:
		.byte	0				# Extended boot loader version
ext_loader_type:
		.byte	0				# Extended boot loader type

cmd_line_ptr:	.long	0				# (Header version 0x0202 or later)
							# If nonzero, a 32-bit pointer
							# to the kernel command line.
							# The command line should be
							# located between the start of
							# setup and the end of low
							# memory (0xa0000), or it may
							# get overwritten before it
							# gets read.  If this field is
							# used, there is no longer
							# anything magical about the
							# 0x90000 segment; the setup
							# can be located anywhere in
							# low memory 0x10000 or higher.
initrd_addr_max:	.long 0x7fffffff
							# (Header version 0x0203 or later)
							# The highest safe address for
							# the contents of an initrd
							# The current kernel allows up to 4 GB,
							# but leave it at 2 GB to avoid
							# possible bootloader bugs.

#define PHYSICAL_ALIGN	0x100000
kernel_alignment:	.long PHYSICAL_ALIGN		#physical addr alignment
							#required for protected mode
							#kernel
relocatable_kernel:	.byte 0

min_alignment:		.byte MIN_KERNEL_ALIGN_LG2	# minimum alignment
xloadflags:		.word 0
cmdline_size:		.long COMMAND_LINE_SIZE-1	#length of the command line,
                                                	#added with boot protocol
                                                	#version 2.06

hardware_subarch:	.long 0				# subarchitecture, added with 2.07
							# default to 0 for normal x86 PC
#define INIT_SIZE		0
#define ZO_input_data		0
#define ZO_z_input_len		0
hardware_subarch_data:	.quad 0
payload_offset:		.long ZO_input_data
payload_length:		.long ZO_z_input_len
setup_data:		.quad 0				# 64-bit physical pointer to
							# single linked list of
							# struct setup_data

pref_address:		.quad LOAD_PHYSICAL_ADDR	# preferred load addr

init_size:		.long INIT_SIZE			# kernel initialization size

handover_offset:	.long 0				# Filled in by build.c

#
# End of setup header #####################################################
##

##
# Now comes the real entry point of kernel setup part.
# Normally, with boot loader, the first instruction executed
# of kernel starts from here.
#

##
# I do not how other boot loaders work, but i know mine.
# Our bootloader always load setup kernel to: 0x9000:0000,
# and protected-mode kernel to: 0x1000:0000.
#

	.section ".entrytext", "ax"
start_of_setup:
	# Sorry, but we are NOT relocatable
	# Please put us to 0x9000:0000
	movw	%cs, %ax
	cmpw	$SETUPSEG, %ax
	jne	setup_not_relocatable
	
	# Normalize everything.
	movw	%ax, %ds
	movw	%ax, %ss
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	cld
	
	# Set the stack behind the setup image.
	# 512 is thought to be enough, if not, call 110
	movw	$__end, %ax
	addw	$STACK_SIZE, %ax
	andw	$~3, %ax		# Make it dword aligned
	jnz	1f
	movw	$0xfffc, %ax		# Make sure it's not zero
1:	movzwl	%ax, %esp		# Clear uppper half of %esp
	
	# Check signature at end of setup
	cmpl	$0x5a5aaa55, setup_sig
	jne	setup_bad

	# Zero the bss
	movw	$__bss_start, %di 
	movw	$__bss_end, %cx 
	xorl	%eax, %eax
	subw    %di, %cx 
	shrw    $2, %cx 
	rep; stosl
	
##
# Jump to kernel setup C code
##
	calll	main

setup_bad:
	movw	$setup_corrupt, %si
	jmp	print
setup_not_relocatable:
	movw	%ax, %ds
	movw	$setup_bad_position, %si
print:
	lodsb
	andb	%al, %al
	jz	setup_die
	movb	$0xe, %ah
	movw	$7, %bx
	int	$0x10
	jmp	print

setup_die:
	hlt
	jmp	setup_die

	.section ".entrydata", "a"
setup_corrupt:
	.ascii "header.S: No setup signature found...\n\r"
	.byte 0
setup_bad_position:
	.ascii "header.S: Setup is not relocatable...\n\r"
	.byte 0
