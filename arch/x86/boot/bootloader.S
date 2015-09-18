#
#	arch/x86/boot/bootloader.S
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
#

#
# This program is a simple boot loader for Sandix. Tiny, about 160 bytes.
# It is supposed to sleep at the first sector of disk, which is the
# conventional MBR sector(without partition information).
#
# This loader trying to load kernel setup to 0x9000:0000, and kernel system to
# 0x1000:0000. After loading, it will jump to 0x9000:0200, which is the start
# of setup(header).
#
# FIXME: Trying to load system to 0x100000
#

BOOTSEG		= 0x07c0	# Bootloader Segment.
SETUPSEG	= 0x9000	# Real-Mode Kernel Image Segment.
SYSSEG		= 0x1000	# Protected-Mode Kernel Image Segment.

SETUP_OFFSET	= 512		# Offset of entry point in Real-mode image.

	.code16
	
	.text
	.globl start
start:
	mov	$BOOTSEG, %ax
	mov	%ax, %ds
	mov	%ax, %es
	mov	%ax, %ss

	##
	# Useful when reading data below
	#
	mov	$SETUPSEG, %ax
	mov	%ax, %fs
	
	##
	# Reset FDC first
	#
	xor	%ah, %ah
	xor	%dl, %dl
	int	$0x13
	
	##
	# Load Kernel Setup and System
	# Load one sector first, then we read the sectors_of_setup
	# and sys_size to decide how many sectors we need to load.
	#

	mov	$SETUPSEG, %ax
	mov	%ax, %es
	xor	%bx, %bx			# 0x9000:0000

	mov	$0x02, %ah			# service 2
	mov	$1, %al				# nr of sectors
	xor	%dx, %dx			# drive 0, head 0
	mov	$0x80, %dl
	mov	$2, %cl				# sector 2, track 0
	int	$0x13
	jc	bs_fail				# CF set on error, go die

	##
	# Read sectors_of_setup
	#
	xor	%ax, %ax
	mov	$0x1f1,%bx			# Defined in header.S
	mov	%fs:(%bx), %al			# sectors_of_setup

	##
	# Save it, in case BIOS screw up
	#
	push	%ax

	##
	# Load the remaining setup
	#
	mov	$SETUPSEG, %bx
	mov	%bx, %es
	mov	$512, %bx			# 0x9000:0200

	mov	$0x02, %ah			# service 2
	xor	%dx, %dx			# drive 0, head 0
	mov	$0x80, %dl
	mov	$3, %cl				# sector 3, track 0
	int	$0x13
	jc	bs_fail				# CF set on error, go die

	##
	# Read sys_size
	#
	xor	%ax, %ax
	mov	$0x1f4, %bx			# Defined in header.S
	movl	%fs:(%bx), %eax			# sys_size, 16-bytes paragraphs

	##
	# Calculate
	# sectors_of_system = (sys_size*16 + 511)/512
	#
	sall	$4, %eax
	addl	$511, %eax
	shrl	$9, %eax			# now %al has sectors
	
bs_load_system:
	mov	$SYSSEG, %bx
	mov	%bx, %es
	xor	%bx, %bx			# 0x1000:0000

	mov	$0x02, %ah			# service 2
	xor	%dx, %dx			# drive 0, head 0
	mov	$0x80, %dl
	pop	%cx				# Restore sectors_of_setup
	add	$3, %cx				# Starting sector number
	int	$0x13
	jc	bs_fail				# CF set on error, go die
	
	# That's all for bootloader!
	# Go to Real-mode setup

	ljmp	$SETUPSEG, $SETUP_OFFSET

bs_fail:
	mov	$bs_msg, %si
1:
	lodsb
	andb	%al, %al
	jz	bs_die
	movb	$0xe, %ah
	movw	$7, %bx
	int	$0x10
	jmp	1b

bs_die:
	hlt
	jmp bs_die

bs_msg:
	.ascii "Fail to load Sandix...\n\r"
	.byte 0
