	.comm	_eax_architectual_perf_version,4,2
	.comm	_eax_nr_of_perf_counter_per_cpu,4,2
	.comm	_eax_bit_width_of_perf_counter,4,2
	.comm	_eax_len_of_ebx_to_enumerate,4,2
	.comm	_ebx_predefined_event_mask,4,2
	.comm	_edx_nr_of_fixed_func_perf_counter,4,2
	.comm	_edx_bit_width_of_fixed_func_perf_counter,4,2
	.text
	.globl _cpuid
_cpuid:
LFB4:
	pushq	%rbp
LCFI0:
	movq	%rsp, %rbp
LCFI1:
	pushq	%rbx
LCFI2:
	movq	%rdi, -32(%rbp)
	movq	%rsi, -40(%rbp)
	movq	%rdx, -48(%rbp)
	movq	%rcx, -56(%rbp)
	movq	-32(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, -12(%rbp)
	movl	-12(%rbp), %eax
# 39 "perf.c" 1
	cpuid
# 0 "" 2
	movl	%ebx, %esi
	movl	%eax, %edi
	movq	-32(%rbp), %rax
	movl	%edi, (%rax)
	movq	-40(%rbp), %rax
	movl	%esi, (%rax)
	movq	-48(%rbp), %rax
	movl	%ecx, (%rax)
	movq	-56(%rbp), %rax
	movl	%edx, (%rax)
	popq	%rbx
	popq	%rbp
LCFI3:
	ret
LFE4:
	.cstring
LC0:
	.ascii "%X %X\12\0"
LC1:
	.ascii "%X\12\0"
	.text
	.globl _rdmsr
_rdmsr:
LFB5:
	pushq	%rbp
LCFI4:
	movq	%rsp, %rbp
LCFI5:
	subq	$32, %rsp
	movl	%edi, -20(%rbp)
	movq	$0, -8(%rbp)
	movl	-20(%rbp), %eax
	movl	%eax, %ecx
# 52 "perf.c" 1
	rdtsc
# 0 "" 2
	movl	%eax, -12(%rbp)
	movl	%edx, -16(%rbp)
	movl	-12(%rbp), %edx
	movl	-16(%rbp), %eax
	movl	%eax, %esi
	leaq	LC0(%rip), %rdi
	movl	$0, %eax
	call	_printf
	cmpq	$0, -8(%rbp)
	jne	L3
	movl	-16(%rbp), %eax
	salq	$32, %rax
	testq	%rax, %rax
	je	L4
L3:
	movl	$1, %eax
	jmp	L5
L4:
	movl	$0, %eax
L5:
	cltq
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, %rsi
	leaq	LC1(%rip), %rdi
	movl	$0, %eax
	call	_printf
	cmpq	$0, -8(%rbp)
	jne	L6
	cmpl	$0, -12(%rbp)
	je	L7
L6:
	movl	$1, %eax
	jmp	L8
L7:
	movl	$0, %eax
L8:
	cltq
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, %rsi
	leaq	LC1(%rip), %rdi
	movl	$0, %eax
	call	_printf
	movq	-8(%rbp), %rax
	leave
LCFI6:
	ret
LFE5:
	.cstring
	.align 3
LC2:
	.ascii "eax = %X ebx = %X ecx = %X edx = %X\12\12\0"
	.align 3
LC3:
	.ascii "Architectual Performance Monitoring Version ID: %u\12\0"
	.align 3
LC4:
	.ascii "Number of general-purpose perf counter per cpu: %u\12\0"
	.align 3
LC5:
	.ascii "Bit width of general-purpose, perf counter reg: %u\12\0"
	.align 3
LC6:
	.ascii "Length of [EBX] bit vector to enumerate events: %u\12\12\0"
	.align 3
LC7:
	.ascii "Number of fixed-function performance counters:    %u\12\0"
	.align 3
LC8:
	.ascii "Bit width of fixed-function performance counters: %u\12\12\0"
	.text
	.globl _get_host_perf_info
_get_host_perf_info:
LFB6:
	pushq	%rbp
LCFI7:
	movq	%rsp, %rbp
LCFI8:
	subq	$16, %rsp
	movl	$10, -4(%rbp)
	leaq	-16(%rbp), %rcx
	leaq	-12(%rbp), %rdx
	leaq	-8(%rbp), %rsi
	leaq	-4(%rbp), %rax
	movq	%rax, %rdi
	call	_cpuid
	movl	-4(%rbp), %eax
	movzbl	%al, %edx
	movq	_eax_architectual_perf_version@GOTPCREL(%rip), %rax
	movl	%edx, (%rax)
	movl	-4(%rbp), %eax
	andl	$65280, %eax
	shrl	$8, %eax
	movl	%eax, %edx
	movq	_eax_nr_of_perf_counter_per_cpu@GOTPCREL(%rip), %rax
	movl	%edx, (%rax)
	movl	-4(%rbp), %eax
	andl	$16711680, %eax
	shrl	$16, %eax
	movl	%eax, %edx
	movq	_eax_bit_width_of_perf_counter@GOTPCREL(%rip), %rax
	movl	%edx, (%rax)
	movl	-4(%rbp), %eax
	shrl	$24, %eax
	movl	%eax, %edx
	movq	_eax_len_of_ebx_to_enumerate@GOTPCREL(%rip), %rax
	movl	%edx, (%rax)
	movl	-8(%rbp), %edx
	movq	_ebx_predefined_event_mask@GOTPCREL(%rip), %rax
	movl	%edx, (%rax)
	movl	-16(%rbp), %eax
	andl	$31, %eax
	movl	%eax, %edx
	movq	_edx_nr_of_fixed_func_perf_counter@GOTPCREL(%rip), %rax
	movl	%edx, (%rax)
	movl	-16(%rbp), %eax
	andl	$8160, %eax
	shrl	$5, %eax
	movl	%eax, %edx
	movq	_edx_bit_width_of_fixed_func_perf_counter@GOTPCREL(%rip), %rax
	movl	%edx, (%rax)
	movl	-16(%rbp), %esi
	movl	-12(%rbp), %ecx
	movl	-8(%rbp), %edx
	movl	-4(%rbp), %eax
	movl	%esi, %r8d
	movl	%eax, %esi
	leaq	LC2(%rip), %rdi
	movl	$0, %eax
	call	_printf
	movq	_eax_architectual_perf_version@GOTPCREL(%rip), %rax
	movl	(%rax), %eax
	movl	%eax, %esi
	leaq	LC3(%rip), %rdi
	movl	$0, %eax
	call	_printf
	movq	_eax_nr_of_perf_counter_per_cpu@GOTPCREL(%rip), %rax
	movl	(%rax), %eax
	movl	%eax, %esi
	leaq	LC4(%rip), %rdi
	movl	$0, %eax
	call	_printf
	movq	_eax_bit_width_of_perf_counter@GOTPCREL(%rip), %rax
	movl	(%rax), %eax
	movl	%eax, %esi
	leaq	LC5(%rip), %rdi
	movl	$0, %eax
	call	_printf
	movq	_eax_len_of_ebx_to_enumerate@GOTPCREL(%rip), %rax
	movl	(%rax), %eax
	movl	%eax, %esi
	leaq	LC6(%rip), %rdi
	movl	$0, %eax
	call	_printf
	movq	_eax_architectual_perf_version@GOTPCREL(%rip), %rax
	movl	(%rax), %eax
	cmpl	$1, %eax
	jbe	L10
	movq	_edx_nr_of_fixed_func_perf_counter@GOTPCREL(%rip), %rax
	movl	(%rax), %eax
	movl	%eax, %esi
	leaq	LC7(%rip), %rdi
	movl	$0, %eax
	call	_printf
	movq	_edx_bit_width_of_fixed_func_perf_counter@GOTPCREL(%rip), %rax
	movl	(%rax), %eax
	movl	%eax, %esi
	leaq	LC8(%rip), %rdi
	movl	$0, %eax
	call	_printf
L10:
	leave
LCFI9:
	ret
LFE6:
	.cstring
LC9:
	.ascii "%d\12\0"
LC10:
	.ascii "rdmsr: %X\12\0"
	.text
	.globl _main
_main:
LFB7:
	pushq	%rbp
LCFI10:
	movq	%rsp, %rbp
LCFI11:
	subq	$16, %rsp
	movl	$8, %esi
	leaq	LC9(%rip), %rdi
	movl	$0, %eax
	call	_printf
	call	_get_host_perf_info
	movl	$390, %edi
	call	_rdmsr
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, %rsi
	leaq	LC10(%rip), %rdi
	movl	$0, %eax
	call	_printf
	leave
LCFI12:
	ret
LFE7:
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
	.quad	LFB4-.
	.set L$set$2,LFE4-LFB4
	.quad L$set$2
	.byte	0
	.byte	0x4
	.set L$set$3,LCFI0-LFB4
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
	.byte	0x83
	.byte	0x3
	.byte	0x4
	.set L$set$6,LCFI3-LCFI2
	.long L$set$6
	.byte	0xc
	.byte	0x7
	.byte	0x8
	.align 3
LEFDE1:
LSFDE3:
	.set L$set$7,LEFDE3-LASFDE3
	.long L$set$7
LASFDE3:
	.long	LASFDE3-EH_frame1
	.quad	LFB5-.
	.set L$set$8,LFE5-LFB5
	.quad L$set$8
	.byte	0
	.byte	0x4
	.set L$set$9,LCFI4-LFB5
	.long L$set$9
	.byte	0xe
	.byte	0x10
	.byte	0x86
	.byte	0x2
	.byte	0x4
	.set L$set$10,LCFI5-LCFI4
	.long L$set$10
	.byte	0xd
	.byte	0x6
	.byte	0x4
	.set L$set$11,LCFI6-LCFI5
	.long L$set$11
	.byte	0xc
	.byte	0x7
	.byte	0x8
	.align 3
LEFDE3:
LSFDE5:
	.set L$set$12,LEFDE5-LASFDE5
	.long L$set$12
LASFDE5:
	.long	LASFDE5-EH_frame1
	.quad	LFB6-.
	.set L$set$13,LFE6-LFB6
	.quad L$set$13
	.byte	0
	.byte	0x4
	.set L$set$14,LCFI7-LFB6
	.long L$set$14
	.byte	0xe
	.byte	0x10
	.byte	0x86
	.byte	0x2
	.byte	0x4
	.set L$set$15,LCFI8-LCFI7
	.long L$set$15
	.byte	0xd
	.byte	0x6
	.byte	0x4
	.set L$set$16,LCFI9-LCFI8
	.long L$set$16
	.byte	0xc
	.byte	0x7
	.byte	0x8
	.align 3
LEFDE5:
LSFDE7:
	.set L$set$17,LEFDE7-LASFDE7
	.long L$set$17
LASFDE7:
	.long	LASFDE7-EH_frame1
	.quad	LFB7-.
	.set L$set$18,LFE7-LFB7
	.quad L$set$18
	.byte	0
	.byte	0x4
	.set L$set$19,LCFI10-LFB7
	.long L$set$19
	.byte	0xe
	.byte	0x10
	.byte	0x86
	.byte	0x2
	.byte	0x4
	.set L$set$20,LCFI11-LCFI10
	.long L$set$20
	.byte	0xd
	.byte	0x6
	.byte	0x4
	.set L$set$21,LCFI12-LCFI11
	.long L$set$21
	.byte	0xc
	.byte	0x7
	.byte	0x8
	.align 3
LEFDE7:
	.subsections_via_symbols
