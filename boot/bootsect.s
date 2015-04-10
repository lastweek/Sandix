#########################################################################
#	bootsect.s 
#	2015/04/01. Created by Shan Yizhou.
#########################################################################
#	bootsect.s is loaded at 0x07c0:0x0000 by BIOS.
#########################################################################	
	
	.code16
	
	.text
	.globl start
start:
#Step1: Relocation itself to INITSEG first.
	movw $0x07c0, %ax
	movw %ax, %ds
	movw $0x9000, %ax
	movw %ax, %es
	movw $256, %cx
	xor %si, %si
	xor %di, %di
	cld
	rep movsw
	ljmp $0x9000, $1f
1:
	movw %cs, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss
	movw $512, %sp	# arbitrary value >>512(Bigger than sector size.)
	
	movb $0x03, %ah	# read cursor pos
	xor %bh, %bh
	int $0x10
	
	movw $23, %cx		# length of msg.
	movw $0x0007, %bx	# page 0, attribute 7.
	movw $msg1, %bp		# pointer to msg.
	movw $0x1301, %ax	# write msg, move cursor.
	int $0x10

color:
	movw $0xb800, %ax
	movw %ax, %ds		#set segment register
	movb $0xff, %al		#attribute byte.
	xor %bx, %bx
1:
	movb $0x66, (%bx)	# char 'f'
	addb $1, %bl
	movb %al, (%bx)		# attribute
	addb $1, %bl
	subb $1, %al
	cmpb $0x0, %al
	jne 1b

#Step2: Load system image from disk
load_sector:

#Step: Transition to 32bit protected mode.
transition32:

#Step: Go to the party.
go_kernel:

msg1:
	.byte 13, 10
	.ascii "Hi, This is Sandix!"
	.byte 13, 10
end:
	nop
	nop
