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

#ifndef _ASM_X86_PROCESSOR_H_
#define _ASM_X86_PROCESSOR_H_

#include <sandix/cache.h>
#include <sandix/types.h>
#include <sandix/stddef.h>
#include <sandix/ptrace.h>
#include <sandix/compiler.h>

#include <asm/asm.h>
#include <asm/page.h>
#include <asm/segment.h>
#include <asm/cpufeature.h>
#include <asm/descriptor.h>
#include <asm/thread_info.h>
#include <asm/processor-flags.h>

/* Detailed CPU infomation */
struct cpuinfo_x86 {
	unsigned char		x86;		/* CPU family */
	unsigned char		x86_vendor;	/* CPU vendor */
	unsigned char		x86_model;
	unsigned char		x86_mask;
#ifdef CONFIG_X86_32
	char			wp_works_ok;	/* It doesn't on 386's */

	/* Problems on some 486Dx4's and old 386's: */
	char			rfu;
	char			pad0;
	char			pad1;
#else
	/* Number of 4K pages in DTLB/ITLB combined(in pages): */
	int			x86_tlbsize;
#endif
	unsigned char		x86_virt_bits;
	unsigned char		x86_phys_bits;
	/* CPUID returned core id bits: */
	unsigned char		x86_coreid_bits;
	/* Max extended CPUID function supported: */
	unsigned int		extended_cpuid_level;
	/* Maximum supported CPUID level, -1=no CPUID: */
	int			cpuid_level;
	unsigned int		x86_capability[NCAPINTS+NBUGINTS];
	char			x86_vendor_id[16];
	char			x86_model_id[64];
	/* in KB - valid for CPUS which support this call: */
	int			x86_cache_size;
	int			x86_cache_alignment;	/* In bytes */
	/* Cache QoS architectural values: */
	int			x86_cache_max_rmid;	/* max index */
	int			x86_cache_occ_scale;	/* scale to bytes */
	int			x86_power;
	unsigned long		loops_per_jiffy;
	/* cpuid returned max cores value: */
	unsigned short		x86_max_cores;
	unsigned short		apicid;
	unsigned short		initial_apicid;
	unsigned short		x86_clflush_size;
	/* number of cores as seen by the OS: */
	unsigned short		booted_cores;
	/* Physical processor id: */
	unsigned short		phys_proc_id;
	/* Core id: */
	unsigned short		cpu_core_id;
	/* Compute unit id */
	unsigned char		compute_unit_id;
	/* Index into per_cpu list: */
	unsigned short		cpu_index;
	unsigned int		microcode;
};

/* Vendor-specific identification */
struct cpu_dev {
	const char		*cpu_vendor;
	const char		*cpu_ident;
	int			cpu_x86_vendor;

	void			(*cpu_early_init)(struct cpuinfo_x86 *);
	void			(*cpu_bsp_init)(struct cpuinfo_x86 *);
	void			(*cpu_init)(struct cpuinfo_x86 *);
	void			(*cpu_identify)(struct cpuinfo_x86 *);
	void			(*cpu_detect_tlb)(struct cpuinfo_x86 *);
	void			(*cpu_bsp_resume)(struct cpuinfo_x86 *);
#ifdef CONFIG_X86_32
	/* Optional vendor specific routine to obtain the cache size. */
	unsigned int		(*legacy_cache_size)(struct cpuinfo_x86 *,
						     unsigned int);
	/* Family/stepping-based lookup table for model names. */
	struct legacy_cpu_model_info {
		int		family;
		const char	*model_names[16];
	} legacy_models[5];
#endif
};

struct _tlb_table {
	unsigned char		descriptor;
	char			tlb_type;
	unsigned int		entries;
	char			info[128];		/* unsigned int ways */
};

#define X86_VENDOR_INTEL	0
#define X86_VENDOR_CYRIX	1
#define X86_VENDOR_AMD		2
#define X86_VENDOR_UMC		3
#define X86_VENDOR_CENTAUR	5
#define X86_VENDOR_TRANSMETA	7
#define X86_VENDOR_NSC		8
#define X86_VENDOR_NUM		9
#define X86_VENDOR_UNKNOWN	0xff

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

extern struct cpuinfo_x86 boot_cpu_info;
extern struct cpu_dev intel_cpu_dev;

void __init early_cpu_init(void);
void __init cpu_init(void);
void print_cpu_info(struct cpuinfo_x86 *c);

void __init x86_configure_nx(void);
void __init x86_report_nx(void);

#endif /* _ASM_X86_PROCESSOR_H_ */
