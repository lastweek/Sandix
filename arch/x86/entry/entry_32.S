/*
 *	Copyright (C) 2015-2016 Yizhou Shan <shan13@purdue.edu>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License along
 *	with this program; if not, write to the Free Software Foundation, Inc.,
 *	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/*
 * entry_32.S contains the system-call and low-level fault and trap handling routines.
 *
 * Stack layout while running C code:
 *
 *	40(%esp) - %oldss
 *	3C(%esp) - %oldesp
 *	38(%esp) - %eflags
 *	34(%esp) - %cs
 *	30(%esp) - %eip
 *	2C(%esp) - orig_eax
 *	28(%esp) - %gs
 *	24(%esp) - %fs
 *	20(%esp) - %es
 *	1C(%esp) - %ds
 *	18(%esp) - %eax
 *	14(%esp) - %ebp
 *	10(%esp) - %edi
 *	 C(%esp) - %esi
 *	 8(%esp) - %edx
 *	 4(%esp) - %ecx
 *	 0(%esp) - %ebx
 */

#include <sandix/linkage.h>

#include <asm/traps.h>
#include <asm/segment.h>
#include <asm/asm-offset.h>

.macro SAVE_ALL
	pushl	%gs
	pushl	%fs
	pushl	%es
	pushl	%ds
	pushl	%eax
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%edx
	pushl	%ecx
	pushl	%ebx
.endm

.macro RESTORE_ALL
	popl	%ebx
	popl	%ecx
	popl	%edx
	popl	%esi
	popl	%edi
	popl	%ebp
	popl	%eax
	popl	%ds
	popl	%es
	popl	%fs
	popl	%gs
.endm

ENTRY(divide_error)
	pushl	$0				# no error code
	pushl	$do_divide_error
	jmp	error_code
END(divide_error)

ENTRY(debug)
	pushl	$0
	pushl	$do_debug
	jmp	error_code
END(debug)

ENTRY(nmi)
	pushl	$0
	pushl	$do_nmi
	jmp	error_code
END(nmi)

ENTRY(int3)
	pushl	$-1				# mask as int
	pushl	$do_int3
	jmp	error_code
END(int3)

ENTRY(overflow)
	pushl	$0
	pushl	$do_overflow
	jmp	error_code
END(overflow)

ENTRY(bounds)
	pushl	$0
	pushl	$do_bounds
	jmp	error_code
END(bounds)

ENTRY(invalid_op)
	pushl	$0
	pushl	$do_invalid_op
	jmp	error_code
END(invalid_op)

ENTRY(device_not_available)
	pushl	$-1
	pushl	$do_device_not_available
	jmp	error_code
END(device_not_available)

ENTRY(double_fault)
	pushl	$do_double_fault
	jmp	error_code
END(double_fault)

ENTRY(coprocessor_segment_overrun)
	pushl	$0
	pushl	$do_coprocessor_segment_overrun
	jmp	error_code
END(coprocessor_segment_overrun)

ENTRY(invalid_TSS)
	pushl	$do_invalid_TSS
	jmp	error_code
END(invalid_TSS)

ENTRY(segment_not_present)
	pushl	$do_segment_not_present
	jmp	error_code
END(segment_not_present)

ENTRY(stack_segment)
	pushl	$do_stack_segment
	jmp	error_code
END(stack_segment)

ENTRY(general_protection)
	pushl	$do_general_protection
	jmp	error_code
END(general_protection)

ENTRY(spurious_interrupt_bug)
	pushl	$0
	pushl	$do_spurious_interrupt_bug
	jmp	error_code
END(spurious_interrupt_bug)

ENTRY(coprocessor_error)
	pushl	$0
	pushl	$do_coprocessor_error
	jmp	error_code
END(coprocessor_error)

ENTRY(alignment_check)
	pushl	$do_alignment_check
	jmp	error_code
END(alignment_check)

ENTRY(machine_check)
	pushl	$0
	pushl	$do_machine_check
	jmp	error_code
END(machine_check)

ENTRY(simd_exception)
	pushl	$0
	pushl	$do_simd_exception
	jmp	error_code
END(simd_exception)

ENTRY(virtualization_exception)
	pushl	$0
	pushl	$do_virtualization_exception
	jmp	error_code
END(virtualization_exception)

ENTRY(reserved)
	pushl	$0
	pushl	$do_reserved
	jmp	error_code
END(reserved)

ENTRY(page_fault)
	pushl	$do_page_fault
	ALIGN
error_code:
	/*
	 * The error code is in orig_eax's slot on the stack
	 * The function address is in %gs's slot on the stack
	 */
	pushl	%fs
	pushl	%es
	pushl	%ds
	pushl	%eax
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%edx
	pushl	%ecx
	pushl	%ebx
	cld

	movl	$(__KERNEL_PERCPU), %ecx
	movl	%ecx, %fs

	movl	$(__USER_DS), %ecx
	movl	%ecx, %ds
	movl	%ecx, %es

	movl	PT_GS(%esp), %edi		# get the function address
	movl	PT_ORIG_EAX(%esp), %edx		# get the error code
	movl	$-1, PT_ORIG_EAX(%esp)		# no syscall to restart
	movl	%esp, %eax			# get the pt_regs pointer

	call	*%edi

	popl	%ebx
	popl	%ecx
	popl	%edx
	popl	%esi
	popl	%edi
	popl	%ebp
	popl	%eax

	popl	%ds
	popl	%es
	popl	%fs
	addl	$8, %esp			# gs and orig_eax

	iret
END(page_fault)

ENTRY(entry_INT80_32)
	pushl	%eax				# orig_eax
	SAVE_ALL
	movl	%esp, %eax			# get the pt_regs pointer
	call	do_syscall_32_irqs_on

	RESTORE_ALL
	addl	$4, %esp			# orig_eax
	iret
END(entry_INT80_32)
