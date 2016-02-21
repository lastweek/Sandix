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
#include <sandix/compiler.h>

#include <asm/page.h>
#include <asm/segment.h>
#include <asm/descriptor.h>
#include <asm/thread_info.h>
#include <asm/processor-flags.h>

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

#define TOP_OF_INIT_STACK \
	((unsigned long)&init_stack + sizeof(init_stack) - TOP_OF_KERNEL_STACK_PADDING)

#ifdef CONFIG_X86_32

/* User space process size: 3GB (default) */
#define TASK_SIZE		PAGE_OFFSET
#define TASK_SIZE_MAX		TASK_SIZE
#define STACK_TOP		TASK_SIZE
#define STACK_TOP_MAX		STACK_TOP

#define INIT_THREAD				\
{						\
	.sp0		= TOP_OF_INIT_STACK,	\
	.sysenter_cs	= __KERNEL_CS,		\
	.io_bitmap_ptr	= NULL,			\
}

#else


#endif /* CONFIG_X86_32 */

static inline void rep_nop(void)
{
	asm volatile ("rep; nop" ::: "memory");
}

static inline void cpu_relax(void)
{
	rep_nop();
}

#endif /* _ASM_X86_PROCESSOR_H_ */
