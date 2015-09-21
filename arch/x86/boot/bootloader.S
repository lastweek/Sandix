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
# Get Disk Geometry Parameters.
# In Modern BIOS(e.g. SeaBIOS), the CHS scheme used in INT 0x13 is
# translated to LBA scheme to address disk. In CHS scheme, Cylinder
# and Head start from 0, Sector start form 1, which means the fisrt
# sector is: 0/0/1. In LBA scheme, the first sector starts from 0.
#
# Using Cylinder/Head/Sector to calculate LBA:
#	LBA = (Cylinder * hPc + Head) * sPt + (Sector - 1)
# Where hPc is heads/cylinder, sPt is sectors/track
#
# In QEMU with SeaBIOS, normally, hPc=15, sPt=63, means one cylinder
# has capacity of 16*63*0.5 KB = 504KB. So, normally, it is NOT
# possible to load cross-cylinder, if you do cross, call 110.

	movb	$8,%ah
	movb	$0x80, %dl			# 0x80 for hard disk
	int	$0x13				# Get Disk Parameters
	
	xorw	%ax, %ax
	movb	%dh, %al
	movw	%ax, hPc			# Store heads/cylinder

	andw	$0x3f, %cx
	movw	%cx, sPt			# Store sectors/track
	
##
# Load Kernel Setup
# Load one sector first, then we read the sectors_of_setup
# and sys_size to decide how many sectors we need to load.
# We have %es = %fs = SETUPSEG = 0x9000

	xorw	%ax, %ax
	xorw	%dx, %dx
	int	$0x13				# Reset FDC
	
	xorw	%bx, %bx			# 0x9000:0000
	movw	$0x0201, %ax			# Service 2, nr_of_sectors = 1
	movw	$2, %cx				# Cylinder = 0, Sector = 2
	movw	$0x80, %dx			# Head = 0, Hard Disk
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
	movb	$0x2, %ah			# Service 2
	movw	$3, %cx				# Cylinder = 0, Sector = 3
	movw	$0x80, %dx			# Head = 0, Hard Disk
	int	$0x13
	jc	bs_fail

##
# Before loading System, calculating the size of system image first.
# Note that the sys_size field is in 16-bytes unit, therefore:
# sectors_of_system = (sys_size * 16 + 511) / 512

	xorw	%ax, %ax
	movw	$0x1f4, %bx			# Defined in header.S
	movl	%fs:(%bx), %eax			# sys_size, 4 bytes long

	sall	$4, %eax			# * 16
	addl	$511, %eax			# + 511
	shrl	$9, %eax			# / 512
	movw	%ax, sremain			# Store it

##
# Loading System in an ugly way.
# First, load the first portion that sleep in head 0. After that, we load
# the remaining. I am NOT good at assembly code, here you go.

	movw	$SYSSEG, %bx
	movw	%bx, %es
	xorw	%bx, %bx			# 0x1000:0000

	popw	%cx				# Restore sectors_of_setup
	addw	$3, %cx				# Starting sector number
	movw	%cx, sread			# I am pretty sure setup is small
	movw	$0, head			# So move to sread directly :)

	movw	sPt, %cx
	subw	sread, %cx
	addw	$1, %cx				# %cx = sPt - sread + 1
	cmpw	sremain, %cx
	jnl	1f				# JUMP if (%cx - sremain) >= 0
	subw	%cx, sremain			# if (%cx < sremain)
	movw	%cx, %ax			# then do sremain -= %cx
	jmp	2f				#      move %cx to %ax
1:
	movw	sremain, %ax			# if (%cx >= sremain)
	movw	$0, sremain			# then move sremain to %ax
						#      move 0 to sremain
2:
	movb	$0x2, %ah			# Service 2
	movw	sread, %cx			# Starting sector number
	movw	$0x80, %dx			# Head = 0, Hard Disk
	int	$0x13
	jc	bs_fail

	movw	sremain, %ax
	testw	%ax, %ax			# Quick Test
	jz	bs_go				# No more? Jump to setup

	movw	sPt, %bx			# Adjust loading base address
	subw	sread, %bx
	addw	$1, %bx				# %bx = sPt - sread + 1
	salw    $9, %bx				# %bx *= 512

##
# Now the remaining sectors after head 0
# (Should merge with the code above) :(

	movw	$1, sread
loop:
	addw	$1, head
	movw	hPc, %cx
	cmpw	head, %cx			# Quick Test if cross-cylinder
	jl	bs_fail				# JUMP if (hPc - head) < 0

	movw	sremain, %cx
	cmpw	sPt, %cx
	jnl	1f				# JUMP if (sremain - sPt) >= 0
	movw	sremain, %ax			# if (sremain < sPt)
	movw	$0, sremain			# then move sremain to %al
	jmp	2f				#      move 0 to sremain
1:
	movw	sPt, %ax			# if (sremain >= sPt)
	subw	%ax, sremain			# then move sPt to %al
						#      sub sPt from sremain
2:
	movb	$0x2, %ah			# Service 2
	movw	sread, %cx			# Starting sector number
	movb	head, %dh			# Head X
	movb	$0x80, %dl			# Hard disk
	int	$0x13
	jc	bs_fail
	
	movw	sremain, %ax
	testw	%ax, %ax			# Quick Test
	jz	bs_go				# No more? Jump to setup
	
	movw	sPt, %ax			# Adjust loading base address
	salw    $9, %ax
	addw	%ax, %bx			# %bx += (sPt * 512)

	jmp	loop				# Next head

# Jump to kernel setup
bs_go:
	ljmp	$SETUPSEG, $SETUP_OFFSET
	
# Some variables for disk operation
hPc:		.word 0				# Heads/Cylinder
sPt:		.word 0				# Sectors/Track
sread:		.word 0				# Starting sector number to read
head:		.word 0				# Current head
cylinder:	.word 0				# Current cylinder(not used)
sremain:	.word 0				# Sectors remained to read

# Simple message, should be more specific
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
bs_msg:
	.ascii "Fail to load Sandix...\n\r"
	.byte 0
