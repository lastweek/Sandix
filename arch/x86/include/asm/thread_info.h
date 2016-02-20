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
 * Low-level architecture-dependent thread information structure definition
 */

#ifndef _ASM_X86_THREAD_INFO_H_
#define _ASM_X86_THREAD_INFO_H_

#ifndef _SANDIX_THREAD_INFO_
# error "Please do not include this file directly"
#endif

/*
 * TOP_OF_KERNEL_STACK_PADDING is a number of unused bytes that we reserve at
 * the top of the kernel stack. We do it because of a nasty 32-bit corner case.
 * On x86_32, the hardware stack frame is variable-length. Except for vm86 mode,
 * struct pt_regs assumes a maximum-length frame. If we enter from CPL 0, the
 * top 8 bytes of pt_regs don't actually exist.
 *
 * x86_64 has a fixed-length stack frame.
 */
#ifdef CONFIG_X86_32
# define TOP_OF_KERNEL_STACK_PADDING	8
#else
# define TOP_OF_KERNEL_STACK_PADDING	0
#endif

struct task_struct;

/**
 * struct thread_info
 * @task: main task structure
 * @flags: low level flags
 * @cpu: current CPU
 */
struct thread_info {
	struct task_struct	*task;
	unsigned long		flags;
	unsigned long		cpu;
};

#define INIT_THREAD_INFO(task)		\
{					\
	.task		= &task,	\
	.flags		= 0,		\
	.cpu		= 0,		\
}

/*
 * Thread information flags
 */

#define TIF_SYSCALL_TRACE	0	/* syscall trace active */
#define TIF_NOTIFY_RESUME	1	/* callback before returning to user */
#define TIF_SIGPENDING		2	/* signal pending */
#define TIF_NEED_RESCHED	3	/* re-scheduling necessary */
#define TIF_SINGLESTEP		4	/* re-enable single step on user return */
#define TIF_SYSCALL_EMU		6	/* syscall emulation active */
#define TIF_SYSCALL_AUDIT	7	/* syscall auditing active */
#define TIF_SECCOMP		8	/* secure computing */
#define TIF_USER_RETURN_NOTIFY	11	/* notify kernel of userspace return */
#define TIF_UPROBE		12	/* breakpointed or singlestepping */
#define TIF_NOTSC		16	/* TSC is not accessible in userland */
#define TIF_IA32		17	/* IA32 compatibility process */
#define TIF_FORK		18	/* ret_from_fork */
#define TIF_NOHZ		19	/* in adaptive nohz mode */
#define TIF_MEMDIE		20	/* is terminating due to OOM killer */
#define TIF_POLLING_NRFLAG	21	/* idle is polling for TIF_NEED_RESCHED */
#define TIF_IO_BITMAP		22	/* uses I/O bitmap */
#define TIF_FORCED_TF		24	/* true if TF in eflags artificially */
#define TIF_BLOCKSTEP		25	/* set when we want DEBUGCTLMSR_BTF */
#define TIF_LAZY_MMU_UPDATES	27	/* task is updating the mmu lazily */
#define TIF_SYSCALL_TRACEPOINT	28	/* syscall tracepoint instrumentation */
#define TIF_ADDR32		29	/* 32-bit address space on 64 bits */
#define TIF_X32			30	/* 32-bit native x86-64 binary */

#define _TIF_SYSCALL_TRACE	(1 << TIF_SYSCALL_TRACE)
#define _TIF_NOTIFY_RESUME	(1 << TIF_NOTIFY_RESUME)
#define _TIF_SIGPENDING		(1 << TIF_SIGPENDING)
#define _TIF_SINGLESTEP		(1 << TIF_SINGLESTEP)
#define _TIF_NEED_RESCHED	(1 << TIF_NEED_RESCHED)
#define _TIF_SYSCALL_EMU	(1 << TIF_SYSCALL_EMU)
#define _TIF_SYSCALL_AUDIT	(1 << TIF_SYSCALL_AUDIT)
#define _TIF_SECCOMP		(1 << TIF_SECCOMP)
#define _TIF_USER_RETURN_NOTIFY	(1 << TIF_USER_RETURN_NOTIFY)
#define _TIF_UPROBE		(1 << TIF_UPROBE)
#define _TIF_NOTSC		(1 << TIF_NOTSC)
#define _TIF_IA32		(1 << TIF_IA32)
#define _TIF_FORK		(1 << TIF_FORK)
#define _TIF_NOHZ		(1 << TIF_NOHZ)
#define _TIF_POLLING_NRFLAG	(1 << TIF_POLLING_NRFLAG)
#define _TIF_IO_BITMAP		(1 << TIF_IO_BITMAP)
#define _TIF_FORCED_TF		(1 << TIF_FORCED_TF)
#define _TIF_BLOCKSTEP		(1 << TIF_BLOCKSTEP)
#define _TIF_LAZY_MMU_UPDATES	(1 << TIF_LAZY_MMU_UPDATES)
#define _TIF_SYSCALL_TRACEPOINT	(1 << TIF_SYSCALL_TRACEPOINT)
#define _TIF_ADDR32		(1 << TIF_ADDR32)
#define _TIF_X32		(1 << TIF_X32)

/*
 * Thread information structure helpers
 */

/*TODO*/
static __always_inline struct thread_info *current_thread_info(void)
{
	return NULL;
}

static __always_inline unsigned long current_stack_pointer(void)
{
	unsigned long sp;
#ifdef CONFIG_X86_32
	asm volatile ("movl %%esp, %0" : "=g" (sp));
#else
	asm volatile ("movq %%rsp, %0" : "=g" (sp));
#endif
	return sp;
}

#endif /* _ASM_X86_THREAD_INFO_H_ */
