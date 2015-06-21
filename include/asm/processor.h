#ifndef _ASM_PROCESSOR_H_
#define _ASM_PROCESSOR_H_

#include <sandix/types.h>
#include <asm/processor-flags.h>

/**
 *  x86 cpu type and hardware bug flags.
 *	Kept separately for each CPU.
 *  Members of this structure are referenced in head.S.
 **/
struct cpuinfo {
	__u8			x86;		/* CPU family */
	__u8			x86_vendor;	/* CPU vendor */
	__u8			x86_model;
	__u8			x86_mask;
	__u8			x86_virt_bits;
	__u8			x86_phys_bits;
	/* cpuid returned max cores value: */
	__u16			x86_max_cores;
	__u16			apicid;
	__u16			initial_apicid;
	__u16			x86_clflush_size;
	/* number of cores as seen by the OS: */
	__u16			booted_cores;
	/* Physical processor id: */
	__u16			phys_proc_id;
	/* Core id: */
	__u16			cpu_core_id;
	/* Index into per_cpu list: */
	__u16			cpu_index;
}__attribute__((packed));


/**
 * Intel x86 hardware task-state segment(tss).
 * Consult SDM-VOL3 section 7.2.1 for details.
 **/
struct x86_tss {
	__u16		link, __linkh;
	
	__u32		esp0;
	__u16		ss0, __ss0h;
	
	__u32		esp1;
	__u16		ss1, __ss1h;
	
	__u32		esp2;
	__u16		ss2, __ss2h;
	
	__u32		cr3;
	
	__u32		eip;
	__u32		eflags;
	__u32		eax;
	__u32		ecx;
	__u32		edx;
	__u32		ebx;
	__u32		esp;
	__u32		ebp;
	__u32		esi;
	__u32		edi;
	
	__u16		es, __esh;
	__u16		cs, __csh;
	__u16		ss, __ssh;
	__u16		ds, __dsh;
	__u16		fs, __fsh;
	__u16		gs, __gsh;
	
	__u16		ldt, __ldth;
	__u16		trace;
	__u16		io_bitmap_base;

} __attribute__((packed));


/**
 * IO-bitmap sizes:
 **/
#define IO_BITMAP_BITS			65536
#define IO_BITMAP_BYTES			(IO_BITMAP_BITS/8)
#define IO_BITMAP_LONGS			(IO_BITMAP_BYTES/sizeof(long))
#define IO_BITMAP_OFFSET		offsetof(struct tss_struct, io_bitmap)
#define INVALID_IO_BITMAP_OFFSET	0x8000


/**
 * cpu specific state for task.
 **/
struct thread_struct {
	/* Cached TLS descriptors: */
	struct desc_struct	tls_array[GDT_ENTRY_TLS_ENTRIES];
	
	__u32		sp0;
	__u32		sp;
	__u32		sysenter_cs;
	__u32		ip;
	__u32		gs;
	
	/* Save middle states of ptrace breakpoints */
	struct perf_event	*ptrace_bps[HBP_NUM];
	
	/* Debug status used for traps, single steps, etc... */
	__u32           debugreg6;
	
	/* Keep track of the exact dr7 value set by the user */
	__u32           ptrace_dr7;
	
	/* Fault info: */
	__u32		cr2;
	__u32		trap_nr;
	__u32		error_code;
	
	/* floating point and extended processor state */
	struct fpu		fpu;
	
	/* IO permissions: */
	__u32		*io_bitmap_ptr;
	__u32		iopl;
	
	/* Max allowed port in the bitmap, in bytes: */
	__u32		io_bitmap_max;
};

#endif /* _ASM_PROCESSOR_H_ */
