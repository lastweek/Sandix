	.cstring
LC0:
	.ascii "unsigned:\11%x %u\12\0"
LC1:
	.ascii "signed  :\11%x %d\12\0"
LC2:
	.ascii "%x %x\12\0"
	.text
	.globl _main
_main:
LFB1:
	pushq	%rbp
LCFI0:
	movq	%rsp, %rbp
LCFI1:
	subq	$16, %rsp
	movl	$0, -4(%rbp)
	movl	-4(%rbp), %eax
	leal	-1(%rax), %edx
	movl	-4(%rbp), %eax
	subl	$1, %eax
	movl	%eax, %esi
	leaq	LC0(%rip), %rdi
	movl	$0, %eax
	call	_printf
	movl	$0, -8(%rbp)
	movl	-8(%rbp), %eax
	leal	-1(%rax), %edx
	movl	-8(%rbp), %eax
	subl	$1, %eax
	movl	%eax, %esi
	leaq	LC1(%rip), %rdi
	movl	$0, %eax
	call	_printf
	movb	$-1, -9(%rbp)
	movzbl	-9(%rbp), %eax
	movzbl	%al, %eax
	movl	%eax, -4(%rbp)
	movsbl	-9(%rbp), %eax
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %edx
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	leaq	LC2(%rip), %rdi
	movl	$0, %eax
	call	_printf
	leave
LCFI2:
	ret
LFE1:
	.section __TEXT,__eh_frame,coalesced,no_toc+strip_static_syms+live_support
EH_frame1:
	.set L$set$0,LECIE1-LSCIE1
	.long L$set$0
LSCIE1:
	.long	0
	.byte	0x1
	.ascii "zR\0"
	.byte	0x1
	.byte	0x78
	.byte	0x10
	.byte	0x1
	.byte	0x10
	.byte	0xc
	.byte	0x7
	.byte	0x8
	.byte	0x90
	.byte	0x1
	.align 3
LECIE1:
LSFDE1:
	.set L$set$1,LEFDE1-LASFDE1
	.long L$set$1
LASFDE1:
	.long	LASFDE1-EH_frame1
	.quad	LFB1-.
	.set L$set$2,LFE1-LFB1
	.quad L$set$2
	.byte	0
	.byte	0x4
	.set L$set$3,LCFI0-LFB1
	.long L$set$3
	.byte	0xe
	.byte	0x10
	.byte	0x86
	.byte	0x2
	.byte	0x4
	.set L$set$4,LCFI1-LCFI0
	.long L$set$4
	.byte	0xd
	.byte	0x6
	.byte	0x4
	.set L$set$5,LCFI2-LCFI1
	.long L$set$5
	.byte	0xc
	.byte	0x7
	.byte	0x8
	.align 3
LEFDE1:
	.subsections_via_symbols
