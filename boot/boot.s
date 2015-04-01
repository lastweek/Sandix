#########################################################################
#	boot.s 
#	2015/04/01. Created by Shan Yizhou.
#########################################################################
#	boot.s is derived from linux-0.01.
#	boot.s is loaded at 0x07c0:0x0000 by BIOS, and moves
#	itself out of the way to address 0x9000:0x0000, and jumps there.
#	
#	It then loads the system at 0x1000:0x0000, using BIOS interrupts.
#	Thereafter it disables all interrupts, moves the system down to 
#	0x0000, changes to protected mode, and calls the start of system.
#
#	System then must RE-initialize the protected mode in it's own tables,
#	and enable interrupts as needed. BTW, I will put the system image right
#	after MBR, which is sector2.
#########################################################################	
#	boot.s MUST be assembled under 16bit mode.
#########################################################################	
	
	.code16gcc
	
	.text
	.globl start
start:
	# Relocation itself to INITSEG first.
	movw BOOTSEG, %ax
	movw %ax, %ds
	movw INITSEG, %ax
	movw %ax, %es
	movw $256, %cx
	xor %si, %si
	xor %di, %di
	rep
	movsw
	jmp $INITSEG, $0x10
go:
	movw %cs, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss
	movw $512, %sp	# arbitrary value >>512(Bigger than sector size.)
	.data
BOOTSEG:
	.word 0x07c0
INITSEG:
	.word 0x9000
SYSSEG:
	.word 0x1000


