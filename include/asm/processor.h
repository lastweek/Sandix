#ifndef _ASM_PROCESSOR_H_
#define _ASM_PROCESSOR_H_

#include <sandix/types.h>
#include <sandix/const.h>

/*
 * EFLAGS BITS
 */
#define X86_EFLAGS_CF_BIT	0 /* Carry Flag */
#define X86_EFLAGS_CF		_BITUL(X86_EFLAGS_CF_BIT)
#define X86_EFLAGS_FIXED_BIT	1 /* Bit 1 - always on */
#define X86_EFLAGS_FIXED	_BITUL(X86_EFLAGS_FIXED_BIT)
#define X86_EFLAGS_PF_BIT	2 /* Parity Flag */
#define X86_EFLAGS_PF		_BITUL(X86_EFLAGS_PF_BIT)
#define X86_EFLAGS_AF_BIT	4 /* Auxiliary carry Flag */
#define X86_EFLAGS_AF		_BITUL(X86_EFLAGS_AF_BIT)
#define X86_EFLAGS_ZF_BIT	6 /* Zero Flag */
#define X86_EFLAGS_ZF		_BITUL(X86_EFLAGS_ZF_BIT)
#define X86_EFLAGS_SF_BIT	7 /* Sign Flag */
#define X86_EFLAGS_SF		_BITUL(X86_EFLAGS_SF_BIT)
#define X86_EFLAGS_TF_BIT	8 /* Trap Flag */
#define X86_EFLAGS_TF		_BITUL(X86_EFLAGS_TF_BIT)
#define X86_EFLAGS_IF_BIT	9 /* Interrupt Flag */
#define X86_EFLAGS_IF		_BITUL(X86_EFLAGS_IF_BIT)
#define X86_EFLAGS_DF_BIT	10 /* Direction Flag */
#define X86_EFLAGS_DF		_BITUL(X86_EFLAGS_DF_BIT)
#define X86_EFLAGS_OF_BIT	11 /* Overflow Flag */
#define X86_EFLAGS_OF		_BITUL(X86_EFLAGS_OF_BIT)
#define X86_EFLAGS_IOPL_BIT	12 /* I/O Privilege Level (2 bits) */
#define X86_EFLAGS_IOPL		(_AC(3,UL) << X86_EFLAGS_IOPL_BIT)
#define X86_EFLAGS_NT_BIT	14 /* Nested Task */
#define X86_EFLAGS_NT		_BITUL(X86_EFLAGS_NT_BIT)
#define X86_EFLAGS_RF_BIT	16 /* Resume Flag */
#define X86_EFLAGS_RF		_BITUL(X86_EFLAGS_RF_BIT)
#define X86_EFLAGS_VM_BIT	17 /* Virtual Mode */
#define X86_EFLAGS_VM		_BITUL(X86_EFLAGS_VM_BIT)
#define X86_EFLAGS_AC_BIT	18 /* Alignment Check/Access Control */
#define X86_EFLAGS_AC		_BITUL(X86_EFLAGS_AC_BIT)
#define X86_EFLAGS_AC_BIT	18 /* Alignment Check/Access Control */
#define X86_EFLAGS_AC		_BITUL(X86_EFLAGS_AC_BIT)
#define X86_EFLAGS_VIF_BIT	19 /* Virtual Interrupt Flag */
#define X86_EFLAGS_VIF		_BITUL(X86_EFLAGS_VIF_BIT)
#define X86_EFLAGS_VIP_BIT	20 /* Virtual Interrupt Pending */
#define X86_EFLAGS_VIP		_BITUL(X86_EFLAGS_VIP_BIT)
#define X86_EFLAGS_ID_BIT	21 /* CPUID detection */
#define X86_EFLAGS_ID		_BITUL(X86_EFLAGS_ID_BIT)

/*
 * Basic CPU control in CR0
 */
#define X86_CR0_PE_BIT		0 /* Protection Enable */
#define X86_CR0_PE		_BITUL(X86_CR0_PE_BIT)
#define X86_CR0_MP_BIT		1 /* Monitor Coprocessor */
#define X86_CR0_MP		_BITUL(X86_CR0_MP_BIT)
#define X86_CR0_EM_BIT		2 /* Emulation */
#define X86_CR0_EM		_BITUL(X86_CR0_EM_BIT)
#define X86_CR0_TS_BIT		3 /* Task Switched */
#define X86_CR0_TS		_BITUL(X86_CR0_TS_BIT)
#define X86_CR0_ET_BIT		4 /* Extension Type */
#define X86_CR0_ET		_BITUL(X86_CR0_ET_BIT)
#define X86_CR0_NE_BIT		5 /* Numeric Error */
#define X86_CR0_NE		_BITUL(X86_CR0_NE_BIT)
#define X86_CR0_WP_BIT		16 /* Write Protect */
#define X86_CR0_WP		_BITUL(X86_CR0_WP_BIT)
#define X86_CR0_AM_BIT		18 /* Alignment Mask */
#define X86_CR0_AM		_BITUL(X86_CR0_AM_BIT)
#define X86_CR0_NW_BIT		29 /* Not Write-through */
#define X86_CR0_NW		_BITUL(X86_CR0_NW_BIT)
#define X86_CR0_CD_BIT		30 /* Cache Disable */
#define X86_CR0_CD		_BITUL(X86_CR0_CD_BIT)
#define X86_CR0_PG_BIT		31 /* Paging */
#define X86_CR0_PG		_BITUL(X86_CR0_PG_BIT)

/*
 * Paging options in CR3
 */
#define X86_CR3_PWT_BIT		3 /* Page Write Through */
#define X86_CR3_PWT		_BITUL(X86_CR3_PWT_BIT)
#define X86_CR3_PCD_BIT		4 /* Page Cache Disable */
#define X86_CR3_PCD		_BITUL(X86_CR3_PCD_BIT)
#define X86_CR3_PCID_MASK	_AC(0x00000fff,UL) /* PCID Mask */

/*
 * Intel CPU features in CR4
 */
#define X86_CR4_VME_BIT		0 /* enable vm86 extensions */
#define X86_CR4_VME		_BITUL(X86_CR4_VME_BIT)
#define X86_CR4_PVI_BIT		1 /* virtual interrupts flag enable */
#define X86_CR4_PVI		_BITUL(X86_CR4_PVI_BIT)
#define X86_CR4_TSD_BIT		2 /* disable time stamp at ipl 3 */
#define X86_CR4_TSD		_BITUL(X86_CR4_TSD_BIT)
#define X86_CR4_DE_BIT		3 /* enable debugging extensions */
#define X86_CR4_DE		_BITUL(X86_CR4_DE_BIT)
#define X86_CR4_PSE_BIT		4 /* enable page size extensions */
#define X86_CR4_PSE		_BITUL(X86_CR4_PSE_BIT)
#define X86_CR4_PAE_BIT		5 /* enable physical address extensions */
#define X86_CR4_PAE		_BITUL(X86_CR4_PAE_BIT)
#define X86_CR4_MCE_BIT		6 /* Machine check enable */
#define X86_CR4_MCE		_BITUL(X86_CR4_MCE_BIT)
#define X86_CR4_PGE_BIT		7 /* enable global pages */
#define X86_CR4_PGE		_BITUL(X86_CR4_PGE_BIT)
#define X86_CR4_PCE_BIT		8 /* enable performance counters at ipl 3 */
#define X86_CR4_PCE		_BITUL(X86_CR4_PCE_BIT)
#define X86_CR4_OSFXSR_BIT	9 /* enable fast FPU save and restore */
#define X86_CR4_OSFXSR		_BITUL(X86_CR4_OSFXSR_BIT)
#define X86_CR4_OSXMMEXCPT_BIT	10 /* enable unmasked SSE exceptions */
#define X86_CR4_OSXMMEXCPT	_BITUL(X86_CR4_OSXMMEXCPT_BIT)
#define X86_CR4_VMXE_BIT	13 /* enable VMX virtualization */
#define X86_CR4_VMXE		_BITUL(X86_CR4_VMXE_BIT)
#define X86_CR4_SMXE_BIT	14 /* enable safer mode (TXT) */
#define X86_CR4_SMXE		_BITUL(X86_CR4_SMXE_BIT)
#define X86_CR4_FSGSBASE_BIT	16 /* enable RDWRFSGS support */
#define X86_CR4_FSGSBASE	_BITUL(X86_CR4_FSGSBASE_BIT)
#define X86_CR4_PCIDE_BIT	17 /* enable PCID support */
#define X86_CR4_PCIDE		_BITUL(X86_CR4_PCIDE_BIT)
#define X86_CR4_OSXSAVE_BIT	18 /* enable xsave and xrestore */
#define X86_CR4_OSXSAVE		_BITUL(X86_CR4_OSXSAVE_BIT)
#define X86_CR4_SMEP_BIT	20 /* enable SMEP support */
#define X86_CR4_SMEP		_BITUL(X86_CR4_SMEP_BIT)
#define X86_CR4_SMAP_BIT	21 /* enable SMAP support */
#define X86_CR4_SMAP		_BITUL(X86_CR4_SMAP_BIT)

/*
 * x86 cpu type and hardware bug flags.
 * Kept separately for each CPU.
 * Members of this structure are referenced in head.S.
 */
struct cpuinfo {
	__u8		x86;		/* CPU family */
	__u8		x86_vendor;	/* CPU vendor */
	__u8		x86_model;
	__u8		x86_mask;
	__u8		x86_virt_bits;
	__u8		x86_phys_bits;
	/* cpuid returned max cores value: */
	__u16		x86_max_cores;
	__u16		apicid;
	__u16		initial_apicid;
	__u16		x86_clflush_size;
	/* number of cores as seen by the OS: */
	__u16		booted_cores;
	/* Physical processor id: */
	__u16		phys_proc_id;
	/* Core id: */
	__u16		cpu_core_id;
	/* Index into per_cpu list: */
	__u16		cpu_index;
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


/*
 * IO-bitmap sizes:
 */
#define IO_BITMAP_BITS			65536
#define IO_BITMAP_BYTES			(IO_BITMAP_BITS/8)
#define IO_BITMAP_LONGS			(IO_BITMAP_BYTES/sizeof(long))
#define IO_BITMAP_OFFSET		offsetof(struct tss_struct, io_bitmap)
#define INVALID_IO_BITMAP_OFFSET	0x8000

/*
 * cpu specific state for task.
 */
struct thread_struct {
	/* Cached TLS descriptors: */
	struct desc_struct	tls_array[GDT_ENTRY_TLS_ENTRIES];
	
	__u32		sp0;
	__u32		sp;
	__u32		sysenter_cs;
	__u32		ip;
	__u32		gs;
	
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
