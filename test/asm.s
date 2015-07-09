	.file	"asm.c"
	.text
	.globl	main
	.type	main, @function
main:
.LFB4:
	.cfi_startproc
	subl	$16, %esp
	.cfi_def_cfa_offset 20
	movl	$1, 12(%esp)
	movl	$3, 8(%esp)
	movl	$15, %edx
/APP
/  32 "asm.c" 1
	lock; orl %edx, 8(%esp)
/  0 "" 2
/NO_APP
	movl	8(%esp), %eax
/APP
/  25 "asm.c" 1
	lock; orl %edx, %eax
/  0 "" 2
/  7 "asm.c" 1
	addl $100, 12(%esp)
/  0 "" 2
/NO_APP
	leal	12(%esp), %eax
	movl	%eax, 4(%esp)
/APP
/  16 "asm.c" 1
	addl $200, 12(%esp)
/  0 "" 2
/NO_APP
	movl	12(%esp), %eax
	addl	$2, %eax
	addl	$16, %esp
	.cfi_def_cfa_offset 4
	ret
	.cfi_endproc
.LFE4:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.7.2"
