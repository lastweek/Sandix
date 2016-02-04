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

#ifndef _ASM_X86_PTRACE_H_
#define _ASM_X86_PTRACE_H_

#ifndef _SANDIX_PTRACE_H_
# error "Please do not include this file directly"
#endif

#ifdef CONFIG_X86_32
struct pt_regs {
	unsigned long bx;
	unsigned long cx;
	unsigned long dx;
	unsigned long si;
	unsigned long di;
	unsigned long bp;
	unsigned long ax;
	unsigned long ds;
	unsigned long es;
	unsigned long fs;
	unsigned long gs;
	unsigned long orig_ax;
	unsigned long ip;
	unsigned long cs;
	unsigned long flags;
	unsigned long sp;
	unsigned long ss;
};
#else
struct pt_regs {
	/*
	 * C ABI says these regs are callee-preserved. They aren't saved on kernel entry
	 * unless syscall needs a complete, fully filled "struct pt_regs".
	 */
	unsigned long r15;
	unsigned long r14;
	unsigned long r13;
	unsigned long r12;
	unsigned long bp;
	unsigned long bx;
	/*
	 * These regs are callee-clobbered.
	 * Always saved on kernel entry.
	 */
	unsigned long r11;
	unsigned long r10;
	unsigned long r9;
	unsigned long r8;
	unsigned long ax;
	unsigned long cx;
	unsigned long dx;
	unsigned long si;
	unsigned long di;
	/*
	 * On syscall entry, this is syscall#.
	 * On CPU exception, this is error code.
	 * On hw interrupt,  this is IRQ number:
	 */
	unsigned long orig_ax;
	/*
	 * Return frame for iretq
	 */
	unsigned long ip;
	unsigned long cs;
	unsigned long flags;
	unsigned long sp;
	unsigned long ss;

	/*
	 * top of stack page
	 */
};
#endif

#ifdef CONFIG_X86_32
unsigned long kernel_stack_pointer(struct pt_regs *regs);
#else
static inline unsigned long kernel_stack_pointer(struct pt_regs *regs)
{
	return regs->sp;
}
#endif

#define GET_IP(regs)	((regs)->ip)
#define GET_FP(regs)	((regs)->bp)
#define GET_USP(regs)	((regs)->sp)

#include <asm-generic/ptrace.h>

#endif /* _ASM_X86_PTRACE_H_ */
