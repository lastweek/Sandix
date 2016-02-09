/*
 *	Copyright (C) 2015-2016 Yizhou Shan <shanyizhou@ict.ac.cn>
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
 * Stack layout while running C code (struct pt_regs):
 *
 *	 0(%esp) - %ebx
 *	 4(%esp) - %ecx
 *	 8(%esp) - %edx
 *	 C(%esp) - %esi
 *	10(%esp) - %edi
 *	14(%esp) - %ebp
 *	18(%esp) - %eax
 *	1C(%esp) - %ds
 *	20(%esp) - %es
 *	24(%esp) - %fs
 *	28(%esp) - %gs
 *	2C(%esp) - orig_eax
 *	30(%esp) - %eip
 *	34(%esp) - %cs
 *	38(%esp) - %eflags
 *	3C(%esp) - %oldesp
 *	40(%esp) - %oldss
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

ENTRY(divide_error)
	pushl	$0				# no error code
	pushl	$do_divide_error
	jmp	error_code
END(divide_error)

ENTRY(debug)

END(debug)

ENTRY(page_fault)
	pushl	$do_page_fault
	ALIGN
error_code:
	/*
	 * the error code is in orig_eax's slot on the stack
	 * the function address is in %gs's slot on the stack
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
	iret
END(page_fault)













