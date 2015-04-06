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
	
	.code16
	
	.text
	.globl _start
_start:
	# Relocation itself to INITSEG first.
	movw $0x07c0, %ax
	movw %ax, %ds
	movw $0x9000, %ax
	movw %ax, %es
	movw $256, %cx
	xor %si, %si
	xor %di, %di
	cld
	rep movsw
	ljmp $0x9000, $go
go:
	movw %cs, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss
	movw $512, %sp	# arbitrary value >>512(Bigger than sector size.)
	
	movb $0x03, %ah	# read cursor pos
	xor %bh, %bh
	int $0x10
	
	movw $22, %cx
	movw $0x0007, %bx	# page 0, attribute 7.
	movw $msg1, %bp
	movw $0x1301, %ax	# write string, move cursor
	int $0x10
	nop
allcolor:
	movw $0xb800, %ax
	movw %ax, %ds		#set segment register
	movb $0xff, %al		#attribute byte.
	xor %bx, %bx
loop:
	movb $0x66, (%bx)	# char 'f'
	addb $1, %bl
	movb %al, (%bx)		# attribute
	addb $1, %bl
	subb $1, %al
	cmpb $0x0, %al
	jne loop
	nop
	jmp end
msg1:
	.byte 13, 10
	.ascii "LOAdINg SysTem ..."
	.byte 13, 10
end:
	nop
	nop
