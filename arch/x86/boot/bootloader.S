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


BOOTSEG		= 0x07c0	# Bootloader Segment.
SETUPSEG	= 0x9000	# Real-Mode Kernel Image Segment.
SYSSEG		= 0x1000	# Protected-Mode Kernel Image Segment.
SETUP_OFFSET	= 512		# Offset of entry point in Real-mode setup

	.code16
	
	.text
	.globl start
start:
##
# Normalize segment registers to avoid bad things.
# segements are as follows: %cs = %ds = %ss = BOOTSEG
# %es = %fs = SETUPSEG, %gs is not used.

	movw	$BOOTSEG, %ax
	movw	%ax, %ds
	movw	%ax, %ss
	movw	$SETUPSEG, %ax
	movw	%ax, %fs
	movw	%ax, %es
##
# Set a reasonable stack. Arbitrary value behind bootloader.
# %sp = 512(Bootloader size) + 8(Needed room for stack)
# Use movl to clear upper 16-bit portion of %esp

	movl	$520, %esp

##
# Get Disk Geometry Parameters. In Modern BIOS(e.g. SeaBIOS),
# the CHS scheme used in INT 0x13 is translated to LBA scheme
# to address disk. We need: sectors/track, heads/cylinder.
#
# Using Cylinder/Head/Sector to calculate LBA:
#	LBA = (Cylinder * hPc + Head) * sPt + (Sector - 1)
# Where hPc is heads/cylinder, sPt is sectors/track
#
# In QEMU with SeaBIOS, normally, hPc=15, sPt=63, means the cylinder
# has capacity of 16*63*0.5 KB = 504KB. So, normally, it is NOT
# possible to cross-cylinder, if you do cross, call 110.

	movb	$8,%ah				# Function number
	movb	$0x80, %dl			# 0x80 for hard disk
	int	$0x13
	
	xorw	%ax, %ax
	movb	%dh, %al
	movw	%ax, hPc			# Store heads/cylinder

	andw	$0x3f, %cx
	movw	%cx, sPt			# Store sectors/track
	
	xorw	%ax, %ax
	xorw	%dx, %dx
	int	$0x13				# Reset FDC
	
##
# Load Kernel Setup
# Load one sector first, then we read the sectors_of_setup
# and sys_size to decide how many sectors we need to load.
# We have %es = %fs = SETUPSEG = 0x9000

	xorw	%bx, %bx			# 0x9000:0000
	movw	$0x0201, %ax			# Service 2, nr_of_sectors = 1
	movw	$2, %cx				# Cylinder = 0, Sector = 2
	andw	$0x80, %dx			# Head = 0, hard disk
	int	$0x13
	jc	bs_fail

	xorw	%ax, %ax
	movw	$0x1f1, %bx			# Defined in header.S
	movb	%fs:(%bx), %al			# sectors_of_setup, 1 byte
	pushw	%ax				# In case BIOS touching

##
# Load the remaining Kernel Setup
# Note that we have confined setup image to 64 sectors(It should be
# 62 sectors, in my opinion), so it is very safe to load it in the
# same head, of course, the same cylinder too.
# Note that we have %al=number of sectors to read already.

	movw	$0x200, %bx			# 0x9000:0200
	movb	$0x02, %ah			# Service 2
	movw	$3, %cx				# Cylinder = 0, Sector = 3
	andw	$0x80, %dx			# Head = 0, hard disk
	int	$0x13
	jc	bs_fail

##
# Before loading System, we calculate the size of system image first.
# Note that the sys_size field is in 16-bytes unit, so:
# sectors_of_system = (sys_size*16 + 511)/512

	xorw	%ax, %ax
	movw	$0x1f4, %bx			# Defined in header.S
	movl	%fs:(%bx), %eax			# sys_size, 4 bytes long

	sall	$4, %eax			# * 16
	addl	$511, %eax			# + 511
	shrl	$9, %eax			# / 512

##
# Now loading System
# eax = secors to read
#
	movw	$SYSSEG, %bx
	movw	%bx, %es
	xorw	%bx, %bx			# 0x1000:0000

	mov	$0x02, %ah			# service 2
	xor	%dx, %dx			# drive 0, head 0
	mov	$0x80, %dl
	pop	%cx				# Restore sectors_of_setup
	add	$3, %cx				# Starting sector number
	int	$0x13
	jc	bs_fail				# CF set on error, go die

##
# Oooooo We are done
# Go to kernel setup

	ljmp	$SETUPSEG, $SETUP_OFFSET

bs_fail:
	mov	$bs_msg, %si
bs_loop:
	lodsb
	andb	%al, %al
	jz	bs_die
	movb	$0xe, %ah
	movw	$7, %bx
	int	$0x10
	jmp	bs_loop

bs_die:
	hlt
	jmp bs_die

##
# heads/cylinder
# sectors/track
hPc:	.word 0
sPt:	.word 0

bs_msg:
	.ascii "Fail to load Sandix...\n\r"
	.byte 0
