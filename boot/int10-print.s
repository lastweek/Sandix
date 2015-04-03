# Use qemu to test INT 0x10.
# 2015/03/22. Created by Shan Yizhou

	.code16
	.text
set_cursor:
	movb $2, %ah
	movb $0x0, %bh		#page 0
	movb $20, %dh		#row
	movb $3, %dl		#col
	int $0x10
	jmp print2
print1:
	movb $9, %ah
	movb $0x66, %al		#char 'f'
	movb $0x0, %bh		#page 0
	movb $0x26, %bl		#attribute
	movb $3, %cl 		#count
	movb $0, %ch
	int $0x10
	xor %ax, %ax
print2:
	jmp allcolor
	movb $0x0e, %ah
	movb $0x66, %al		#char 'f'
	movb $0x87, %bl		#attribute
	int $0x10
#
# Print 0xff--0x00 color attribute
#
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
