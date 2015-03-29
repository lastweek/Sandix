	.code16gcc
	.comm	_var_addr,4,2
	.cstring
LC0:
	.ascii "%\0"
LC1:
	.ascii "%d\0"
	.text
	.globl _foo
_foo:
LFB1:
	pushl	%ebp
LCFI0:
	movl	%esp, %ebp
LCFI1:
	pushl	%ebx
	subl	$20, %esp
LCFI2:
	call	___x86.get_pc_thunk.bx
L1$pb:
	movl	8(%ebp), %eax
	movl	%eax, -12(%ebp)
	leal	L_var_addr$non_lazy_ptr-L1$pb(%ebx), %eax
	movl	(%eax), %eax
	leal	40(%ebp), %edx
	movl	%edx, (%eax)
	jmp	L2
L11:
	movl	-12(%ebp), %eax
	movzbl	(%eax), %eax
	cmpb	$37, %al
	jne	L3
	addl	$1, -12(%ebp)
	movl	-12(%ebp), %eax
	movzbl	(%eax), %eax
	testb	%al, %al
	jne	L4
	subl	$12, %esp
	leal	LC0-L1$pb(%ebx), %eax
	pushl	%eax
	call	_printf
	addl	$16, %esp
	jmp	L5
L4:
	movl	-12(%ebp), %eax
	movzbl	(%eax), %eax
	movsbl	%al, %eax
	cmpl	$99, %eax
	je	L7
	cmpl	$100, %eax
	je	L8
	jmp	L9
L8:
	leal	L_var_addr$non_lazy_ptr-L1$pb(%ebx), %eax
	movl	(%eax), %eax
	movl	(%eax), %eax
	movl	(%eax), %eax
	movl	%eax, -16(%ebp)
	leal	L_var_addr$non_lazy_ptr-L1$pb(%ebx), %eax
	movl	(%eax), %eax
	movl	(%eax), %eax
	leal	8(%eax), %edx
	leal	L_var_addr$non_lazy_ptr-L1$pb(%ebx), %eax
	movl	(%eax), %eax
	movl	%edx, (%eax)
	subl	$8, %esp
	pushl	-16(%ebp)
	leal	LC1-L1$pb(%ebx), %eax
	pushl	%eax
	call	_printf
	addl	$16, %esp
	jmp	L9
L7:
	leal	L_var_addr$non_lazy_ptr-L1$pb(%ebx), %eax
	movl	(%eax), %eax
	movl	(%eax), %eax
	movzbl	(%eax), %eax
	movb	%al, -17(%ebp)
	leal	L_var_addr$non_lazy_ptr-L1$pb(%ebx), %eax
	movl	(%eax), %eax
	movl	(%eax), %eax
	leal	8(%eax), %edx
	leal	L_var_addr$non_lazy_ptr-L1$pb(%ebx), %eax
	movl	(%eax), %eax
	movl	%edx, (%eax)
	movsbl	-17(%ebp), %eax
	subl	$12, %esp
	pushl	%eax
	call	_putchar
	addl	$16, %esp
	nop
L9:
	jmp	L10
L3:
	movl	-12(%ebp), %eax
	movzbl	(%eax), %eax
	movsbl	%al, %eax
	subl	$12, %esp
	pushl	%eax
	call	_putchar
	addl	$16, %esp
L10:
	addl	$1, -12(%ebp)
L2:
	movl	-12(%ebp), %eax
	movzbl	(%eax), %eax
	testb	%al, %al
	jne	L11
L5:
	movl	$0, %eax
	movl	-4(%ebp), %ebx
	leave
LCFI3:
	ret
LFE1:
	.cstring
LC2:
	.ascii "Hello!! n = %d\12 m = %d\12\0"
	.text
	.globl _main
_main:
LFB2:
	pushl	%ebp
LCFI4:
	movl	%esp, %ebp
LCFI5:
	pushl	%ebx
	subl	$4, %esp
LCFI6:
	call	___x86.get_pc_thunk.bx
L2$pb:
	subl	$4, %esp
	pushl	$200
	pushl	$100
	leal	LC2-L2$pb(%ebx), %eax
	pushl	%eax
	call	_foo
	addl	$16, %esp
	movl	$0, %eax
	movl	-4(%ebp), %ebx
	leave
LCFI7:
	ret
LFE2:
	.section __TEXT,__textcoal_nt,coalesced,pure_instructions
	.weak_definition	___x86.get_pc_thunk.bx
	.private_extern	___x86.get_pc_thunk.bx
___x86.get_pc_thunk.bx:
LFB3:
	movl	(%esp), %ebx
	ret
LFE3:
	.section __TEXT,__eh_frame,coalesced,no_toc+strip_static_syms+live_support
EH_frame1:
	.set L$set$0,LECIE1-LSCIE1
	.long L$set$0
LSCIE1:
	.long	0
	.byte	0x1
	.ascii "zR\0"
	.byte	0x1
	.byte	0x7c
	.byte	0x8
	.byte	0x1
	.byte	0x10
	.byte	0xc
	.byte	0x5
	.byte	0x4
	.byte	0x88
	.byte	0x1
	.align 2
LECIE1:
LSFDE1:
	.set L$set$1,LEFDE1-LASFDE1
	.long L$set$1
LASFDE1:
	.long	LASFDE1-EH_frame1
	.long	LFB1-.
	.set L$set$2,LFE1-LFB1
	.long L$set$2
	.byte	0
	.byte	0x4
	.set L$set$3,LCFI0-LFB1
	.long L$set$3
	.byte	0xe
	.byte	0x8
	.byte	0x84
	.byte	0x2
	.byte	0x4
	.set L$set$4,LCFI1-LCFI0
	.long L$set$4
	.byte	0xd
	.byte	0x4
	.byte	0x4
	.set L$set$5,LCFI2-LCFI1
	.long L$set$5
	.byte	0x83
	.byte	0x3
	.byte	0x4
	.set L$set$6,LCFI3-LCFI2
	.long L$set$6
	.byte	0xc4
	.byte	0xc3
	.byte	0xc
	.byte	0x5
	.byte	0x4
	.align 2
LEFDE1:
LSFDE3:
	.set L$set$7,LEFDE3-LASFDE3
	.long L$set$7
LASFDE3:
	.long	LASFDE3-EH_frame1
	.long	LFB2-.
	.set L$set$8,LFE2-LFB2
	.long L$set$8
	.byte	0
	.byte	0x4
	.set L$set$9,LCFI4-LFB2
	.long L$set$9
	.byte	0xe
	.byte	0x8
	.byte	0x84
	.byte	0x2
	.byte	0x4
	.set L$set$10,LCFI5-LCFI4
	.long L$set$10
	.byte	0xd
	.byte	0x4
	.byte	0x4
	.set L$set$11,LCFI6-LCFI5
	.long L$set$11
	.byte	0x83
	.byte	0x3
	.byte	0x4
	.set L$set$12,LCFI7-LCFI6
	.long L$set$12
	.byte	0xc4
	.byte	0xc3
	.byte	0xc
	.byte	0x5
	.byte	0x4
	.align 2
LEFDE3:
LSFDE5:
	.set L$set$13,LEFDE5-LASFDE5
	.long L$set$13
LASFDE5:
	.long	LASFDE5-EH_frame1
	.long	LFB3-.
	.set L$set$14,LFE3-LFB3
	.long L$set$14
	.byte	0
	.align 2
LEFDE5:
	.section __IMPORT,__pointers,non_lazy_symbol_pointers
L_var_addr$non_lazy_ptr:
	.indirect_symbol _var_addr
	.long	0
	.subsections_via_symbols
