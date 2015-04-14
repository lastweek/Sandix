	.file	"main.c"
	.code16gcc
	.text
	.globl	main
	.type	main, @function
main:
	pushl	%ebp
	movl	%esp, %ebp
	movw $0x0007, %bx
	movw $0x0001, %cx
	movb $0x0e, %ah
	movb $66, %al
	int $0x10
	popl	%ebp
	ret
