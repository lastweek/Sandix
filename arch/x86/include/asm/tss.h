/*
 *	Copyright (C) 2016 Yizhou Shan <shan13@purdue.edu>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 */

#ifndef _ASM_X86_INCLUDE_TSS_H_
#define _ASM_X86_INCLUDE_TSS_H_

/*
 * Task-State Segment (TSS)
 * The TSS specifies the segments that make up the task execution space and
 * provides a storage place for task state information.
 *
 * The TSS, like all other segments, is defined by a segment descriptor.
 * So we need to set an entry for TSS in GDT table at initialization time.
 *
 * Check SDM Chapter 7 Task Management for more information.
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

/* IO-bitmap sizes */
#define IO_BITMAP_BITS			65536
#define IO_BITMAP_BYTES			(IO_BITMAP_BITS/8)
#define IO_BITMAP_LONGS			(IO_BITMAP_BYTES/sizeof(long))
#define IO_BITMAP_OFFSET		offsetof(struct tss_struct, io_bitmap)
#define INVALID_IO_BITMAP_OFFSET	0x8000

struct tss_struct {
	/*
	 * The hardware state:
	 */
	struct x86_hw_tss	x86_tss;

	/*
	 * The extra 1 is there because the CPU will access an
	 * additional byte beyond the end of the IO permission
	 * bitmap. The extra byte must be all 1 bits, and must
	 * be within the limit.
	 */
	unsigned long		io_bitmap[IO_BITMAP_LONGS + 1];

#ifdef CONFIG_X86_32
	/*
	 * Space for the temporary SYSENTER stack:
	 */
	unsigned long		SYSENTER_stack_canary;
	unsigned long		SYSENTER_stack[64];
#endif
} __cacheline_aligned;

#endif /* _ASM_X86_INCLUDE_TSS_H_ */
