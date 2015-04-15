	.file	"main.c"
/APP
	.code16
/NO_APP
	.text
	.globl	bios_putchar
	.type	bios_putchar, @function
bios_putchar:
.LFB0:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	pushl	%ebx
	.cfi_offset 3, -12
/APP
/  37 "main.c" 1
	movb 8(%ebp), %al
	movb $0x0e, %ah
	movw $0x0007, %bx
	movw $0x0001, %cx
	int $0x10
	
/  0 "" 2
/NO_APP
	popl	%ebx
	.cfi_restore 3
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	bios_putchar, .-bios_putchar
	.globl	putchar
	.type	putchar, @function
putchar:
.LFB1:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	cmpl	$10, 8(%ebp)
	jne	.L3
	movl	$13, (%esp)
	calll	putchar
.L3:
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	calll	bios_putchar
	movl %ebp, %esp
	popl %ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE1:
	.size	putchar, .-putchar
	.globl	puts
	.type	puts, @function
puts:
.LFB2:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	jmp	.L5
.L6:
	movl	8(%ebp), %eax
	movb	(%eax), %al
	movsbl	%al, %eax
	incl	8(%ebp)
	movl	%eax, (%esp)
	calll	putchar
.L5:
	movl	8(%ebp), %eax
	movb	(%eax), %al
	testb	%al, %al
	jne	.L6
	movl %ebp, %esp
	popl %ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE2:
	.size	puts, .-puts
	.section	.rodata
.LC0:
	.string	"\nHello World!"
	.text
	.globl	main
	.type	main, @function
main:
.LFB3:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	andl	$-16, %esp
	subl	$16, %esp
	movl	$.LC0, (%esp)
	calll	puts
	movl	$65, (%esp)
	calll	bios_putchar
	movl %ebp, %esp
	popl %ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE3:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.7.2"
