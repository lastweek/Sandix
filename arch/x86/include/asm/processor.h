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

#ifndef _ASM_X86_PROCESSOR_H_
#define _ASM_X86_PROCESSOR_H_

#include <sandix/cache.h>
#include <sandix/types.h>
#include <sandix/stddef.h>
#include <sandix/ptrace.h>
#include <sandix/compiler.h>

#include <asm/page.h>
#include <asm/segment.h>
#include <asm/descriptor.h>
#include <asm/thread_info.h>
#include <asm/processor-flags.h>

static inline void rep_nop(void)
{
	asm volatile ("rep; nop" ::: "memory");
}

static inline void cpu_relax(void)
{
	rep_nop();
}

#define X86_VENDOR_INTEL	0
#define X86_VENDOR_CYRIX	1
#define X86_VENDOR_AMD		2
#define X86_VENDOR_UMC		3
#define X86_VENDOR_CENTAUR	5
#define X86_VENDOR_TRANSMETA	7
#define X86_VENDOR_NSC		8
#define X86_VENDOR_NUM		9
#define X86_VENDOR_UNKNOWN	0xff

/*
 * TSS defined by the hardware
 */

#ifdef CONFIG_X86_32
struct x86_hw_tss {
	unsigned short		back_link, __blh;
	unsigned long		sp0;
	unsigned short		ss0, __ss0h;
	unsigned long		sp1;

	/*
	 * We don't use ring 1, so ss1 is a convenient scratch space in
	 * the same cacheline as sp0.  We use ss1 to cache the value in
	 * MSR_IA32_SYSENTER_CS.  When we context switch
	 * MSR_IA32_SYSENTER_CS, we first check if the new value being
	 * written matches ss1, and, if it's not, then we wrmsr the new
	 * value and update ss1.
	 *
	 * The only reason we context switch MSR_IA32_SYSENTER_CS is
	 * that we set it to zero in vm86 tasks to avoid corrupting the
	 * stack if we were to go through the sysenter path from vm86
	 * mode.
	 */
	unsigned short		ss1;	/* MSR_IA32_SYSENTER_CS */

	unsigned short		__ss1h;
	unsigned long		sp2;
	unsigned short		ss2, __ss2h;
	unsigned long		__cr3;
	unsigned long		ip;
	unsigned long		flags;
	unsigned long		ax;
	unsigned long		cx;
	unsigned long		dx;
	unsigned long		bx;
	unsigned long		sp;
	unsigned long		bp;
	unsigned long		si;
	unsigned long		di;
	unsigned short		es, __esh;
	unsigned short		cs, __csh;
	unsigned short		ss, __ssh;
	unsigned short		ds, __dsh;
	unsigned short		fs, __fsh;
	unsigned short		gs, __gsh;
	unsigned short		ldt, __ldth;
	unsigned short		trace;
	unsigned short		io_bitmap_base;

} __packed __cacheline_aligned;
#else
struct x86_hw_tss {
	unsigned int		reserved1;
	unsigned long		sp0;
	unsigned long		sp1;
	unsigned long		sp2;
	unsigned long		reserved2;
	unsigned long		ist[7];
	unsigned int		reserved3;
	unsigned int		reserved4;
	unsigned short		reserved5;
	unsigned short		io_bitmap_base;

} __packed __cacheline_aligned;
#endif

/*
 * IO-bitmap sizes:
 */
#define IO_BITMAP_BITS			65536
#define IO_BITMAP_BYTES			(IO_BITMAP_BITS/8)
#define IO_BITMAP_LONGS			(IO_BITMAP_BYTES/sizeof(long))
#define IO_BITMAP_OFFSET		offsetof(struct tss_struct, io_bitmap)
#define INVALID_IO_BITMAP_OFFSET	0x8000

struct tss_struct {
	/* the hardware state */
	struct x86_hw_tss	x86_tss;

	/*
	 * The extra 1 is there because the CPU will access an
	 * additional byte beyond the end of the IO permission
	 * bitmap. The extra byte must be all 1 bits, and must
	 * be within the limit.
	 */
	unsigned long		io_bitmap[IO_BITMAP_LONGS + 1];
} __cacheline_aligned;

struct irq_stack {
	unsigned long 		stack[THREAD_SIZE/sizeof(unsigned long)];
} __aligned(THREAD_SIZE);

struct thread_struct {
	struct desc_struct	tls_array[GDT_ENTRY_TLS_ENTRIES];

	unsigned long		sp0;
	unsigned long		sp;

#ifdef CONFIG_X86_32
	unsigned long		sysenter_cs;
	unsigned long		ip;
#else
	unsigned short		es;
	unsigned short		ds;
	unsigned short		fsindex;
	unsigned short		gsindex;
	unsigned long		fs;
#endif

	unsigned long		gs;

	/* fault info */
	unsigned long		cr2;
	unsigned long		trap_nr;
	unsigned long		error_code;

	/* IO permissions */
	unsigned long		*io_bitmap_ptr;
	unsigned long		iopl;

	/* max allowed port in bitmap, in bytes */
	unsigned int		io_bitmap_max;
};

/*
 * TOP_OF_KERNEL_STACK_PADDING is a number of unused bytes that we reserve at
 * the top of the kernel stack. We do it because of a nasty 32-bit corner case.
 * On x86_32, the hardware stack frame is variable-length. On x86_64, the
 * hardware has a fixed-length stack frame.
 *
 * struct pt_regs assumes a maximum-length frame. Hence, in x86_32, we have to
 * reserve some bytes to avoid cross-boundary accesses, which may cause page
 * fault:
 *  - If we enter from CPL 0, the top 8 bytes of pt_regs don't actually exist.
 *  - In vm86 mode, the hardware frame is much longer still, so add 16 bytes to
 *    make room for the real-mode segments.
 */
#ifdef CONFIG_X86_32
# ifdef CONFIG_VM86
#  define TOP_OF_KERNEL_STACK_PADDING	16
# else
#  define TOP_OF_KERNEL_STACK_PADDING	8
# endif
#else
# define TOP_OF_KERNEL_STACK_PADDING	0
#endif

#define __TOP_OF_INIT_STACK	((unsigned long)&init_stack + sizeof(init_stack))
#define TOP_OF_INIT_STACK	(__TOP_OF_INIT_STACK - TOP_OF_KERNEL_STACK_PADDING)

#ifdef CONFIG_X86_32

/* User space process size: 3GB (default) */
#define TASK_SIZE		PAGE_OFFSET
#define TASK_SIZE_MAX		TASK_SIZE
#define STACK_TOP		TASK_SIZE
#define STACK_TOP_MAX		STACK_TOP

#define INIT_THREAD							\
{									\
	.sp0		= TOP_OF_INIT_STACK,				\
	.sysenter_cs	= __KERNEL_CS,					\
	.io_bitmap_ptr	= NULL,						\
}

/*
 * TOP_OF_KERNEL_STACK_PADDING reserves some bytes on top of the Ring 0 stack.
 * This is necessary to guarantee that the entire "struct pt_regs" is accessible
 * even if the CPU haven't stored the SS/ESP registers on the stack (interrupt
 * gate does not save these registers when switching to the same priv ring).
 * Therefore beware: accessing the SS/ESP fields of the "struct pt_regs" is
 * possible, but they may contain the completely wrong values.
 *
 * Also, we have to use macro instead of inline function here, to avoid
 * infinite including hell (try include sched.h then you know why).
 */
#define task_to_pt_regs(task)						\
({									\
	unsigned long __regs = (unsigned long)task_stack_page(task);	\
	__regs += THREAD_SIZE - TOP_OF_KERNEL_STACK_PADDING;		\
	(struct pt_regs *)__regs - 1;					\
})

#else /* x86_64 */

/*
 * User space process size. 47bits minus one guard page.  The guard
 * page is necessary on Intel CPUs: if a SYSCALL instruction is at
 * the highest possible canonical userspace address, then that
 * syscall will enter the kernel with a non-canonical return
 * address, and SYSRET will explode dangerously.  We avoid this
 * particular problem by preventing anything from being mapped
 * at the maximum canonical address.
 */
#define TASK_SIZE_MAX		((1UL << 47) - PAGE_SIZE)
#define TASK_SIZE		TASK_SIZE_MAX
#define STACK_TOP		TASK_SIZE
#define STACK_TOP_MAX		STACK_TOP

#define INIT_THREAD							\
{									\
	.sp0 = TOP_OF_INIT_STACK,					\
}

#define task_to_pt_regs(task)						\
({									\
	((struct pt_regs *)(task)->thread.sp0 - 1);			\
})

#endif /* CONFIG_X86_32 */

#define pt_regs_to_thread_info(regs)					\
({									\
	unsigned long top_of_stack =					\
		(unsigned long)(regs+1) + TOP_OF_KERNEL_STACK_PADDING;	\
	(struct thread_info *)(top_of_stack - THREAD_SIZE);		\
})

#define pt_regs_to_task(regs)						\
({									\
	pt_regs_to_thread_info(regs)->task;				\
})

#endif /* _ASM_X86_PROCESSOR_H_ */
