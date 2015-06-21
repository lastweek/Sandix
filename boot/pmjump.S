/*
 *	2014/04/27 Created by Shan Yizhou.
 *	pmjump.S: The actual transition into protected mode
 */

#include <asm/segment.h>

	.text
	.code16

	.globl protected_mode_jump
	.type protected_mode_jump, @function
protected_mode_jump:

	# Pointer to boot_params struct
	movl	%edx, %esi
	
	# Self-modify the offset of ljmp
	xorl	%ebx, %ebx
	movw	%cs, %bx
	shll	$4, %ebx
	addl	%ebx, 2f
	
	# Short jump to serialize on 386/486
	# Cause above is self-modifying code,
	# a serializing instruction[like cpuid] is needed
	# before excute the new instruction.
	jmp	1f	
1:
	movw	$__BOOT_DS, %cx
	movw	$__BOOT_TSS, %di

	# Set Protected-mode bit in CR0
	movl	%cr0, %edx
	orb		$0x1, %dl
	movl	%edx, %cr0

	# Long jump tp Protected-mode
	.byte	0x66, 0xea			# ljmpl opcode
2:	.long	in_pm32				# offset
	.word	__BOOT_CS			# segment


	.code32
	.section ".text32","ax"
	.type in_pm32, @function
in_pm32:

	# Set up segments
	movl	%ecx, %ds
	movl	%ecx, %es
	movl	%ecx, %fs
	movl	%ecx, %gs
	movl	%ecx, %ss

	# The 32-bit code sets up its own stack, but this way we do have
	# a valid stack if some debugging hack wants to use it.
	addl	%ebx, %esp

	# Set up TR to make Intel VT happy
	ltr	%di

	xorl	%ebx, %ebx
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	xorl	%ebp, %ebp
	xorl	%edi, %edi

	# Set up LDTR to make Intel VT happy
	lldt	%cx

	# Jump to the 32-bit entrypoint, 0x10000
	jmpl	*%eax
