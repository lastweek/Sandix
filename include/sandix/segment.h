#ifndef _SANDIX_X86_SEGMENT_H
#define _SANDIX_X86_SEGMENT_H

#include <sandix/const.h>

/* Constructor for a segment GDT/LDT entry */
#define GDT_ENTRY(flags, base, limit)			\
	((((base)  & _AC(0xff000000,ULL)) << 32) |	\
	 (((flags) & _AC(0x0000f0ff,ULL)) << 40) |	\
	 (((limit) & _AC(0x000f0000,ULL)) << 32) |	\
	 (((base)  & _AC(0x00ffffff,ULL)) << 16) |	\
	 (((limit) & _AC(0x0000ffff,ULL))))

/* Temporary GDT entries for booting only */
#define GDT_ENTRY_BOOT_CS	2
#define GDT_ENTRY_BOOT_DS	3
#define GDT_ENTRY_BOOT_TSS	4

// * 8 equals to << 3
#define __BOOT_CS		(GDT_ENTRY_BOOT_CS  * 8)
#define __BOOT_DS		(GDT_ENTRY_BOOT_DS  * 8)
#define __BOOT_TSS		(GDT_ENTRY_BOOT_TSS * 8)

/*
 * The layout of the per-CPU GDT under Sandix:
 *
 *   0 - null
 *   1 - reserved
 *   2 - reserved
 *   3 - reserved
 *
 *   4 - unused			<==== new cacheline
 *   5 - unused
 *
 *  ------- start of TLS (Thread-Local Storage) segments:
 *
 *   6 - TLS segment #1			[ glibc's TLS segment ]
 *   7 - TLS segment #2			[ Wine's %fs Win32 segment ]
 *   8 - TLS segment #3
 *   9 - reserved
 *  10 - reserved
 *  11 - reserved
 *
 *  ------- start of kernel segments:
 *
 *  12 - kernel code segment		<==== new cacheline
 *  13 - kernel data segment
 *  14 - default user CS
 *  15 - default user DS
 *  16 - TSS
 *  17 - LDT
 *
 *  26 - ESPFIX small SS
 *  27 - per-cpu			[ offset to per-cpu data area ]
 *  28 - stack_canary-20		[ for stack protector ]
 *  29 - unused
 *  30 - unused
 *  31 - TSS for double fault handler
 */
#define GDT_ENTRY_TLS_MIN	6
#define GDT_ENTRY_TLS_MAX 	(GDT_ENTRY_TLS_MIN + GDT_ENTRY_TLS_ENTRIES - 1)

#define GDT_ENTRY_DEFAULT_USER_CS	14

#define GDT_ENTRY_DEFAULT_USER_DS	15

#define GDT_ENTRY_KERNEL_BASE		(12)

#define GDT_ENTRY_KERNEL_CS		(GDT_ENTRY_KERNEL_BASE+0)

#define GDT_ENTRY_KERNEL_DS		(GDT_ENTRY_KERNEL_BASE+1)

#define GDT_ENTRY_TSS			(GDT_ENTRY_KERNEL_BASE+4)
#define GDT_ENTRY_LDT			(GDT_ENTRY_KERNEL_BASE+5)

#define GDT_ENTRY_PNPBIOS_BASE		(GDT_ENTRY_KERNEL_BASE+6)
#define GDT_ENTRY_APMBIOS_BASE		(GDT_ENTRY_KERNEL_BASE+11)

#define GDT_ENTRY_ESPFIX_SS		(GDT_ENTRY_KERNEL_BASE+14)
#define __ESPFIX_SS			(GDT_ENTRY_ESPFIX_SS*8)

#define GDT_ENTRY_PERCPU		(GDT_ENTRY_KERNEL_BASE+15)
#ifdef CONFIG_SMP
#define __KERNEL_PERCPU (GDT_ENTRY_PERCPU * 8)
#else
#define __KERNEL_PERCPU 0
#endif

#define GDT_ENTRY_STACK_CANARY		(GDT_ENTRY_KERNEL_BASE+16)
#ifdef CONFIG_CC_STACKPROTECTOR
#define __KERNEL_STACK_CANARY		(GDT_ENTRY_STACK_CANARY*8)
#else
#define __KERNEL_STACK_CANARY		0
#endif

#define GDT_ENTRY_DOUBLEFAULT_TSS	31

/*
 * The GDT has 32 entries
 */
#define GDT_ENTRIES 32

/* Bottom two bits of selector give the ring privilege level */
#define SEGMENT_RPL_MASK	0x3
/* Bit 2 is table indicator (LDT/GDT) */
#define SEGMENT_TI_MASK		0x4

/* User mode is privilege level 3 */
#define USER_RPL		0x3
/* LDT segment has TI set, GDT has it cleared */
#define SEGMENT_LDT		0x4
#define SEGMENT_GDT		0x0

#define __KERNEL_CS	(GDT_ENTRY_KERNEL_CS*8)
#define __KERNEL_DS	(GDT_ENTRY_KERNEL_DS*8)
#define __USER_DS	(GDT_ENTRY_DEFAULT_USER_DS*8+3)
#define __USER_CS	(GDT_ENTRY_DEFAULT_USER_CS*8+3)
#ifndef CONFIG_PARAVIRT
#define get_kernel_rpl()  0
#endif

/* User mode is privilege level 3 */
#define USER_RPL		0x3
/* LDT segment has TI set, GDT has it cleared */
#define SEGMENT_LDT		0x4
#define SEGMENT_GDT		0x0

/* Bottom two bits of selector give the ring privilege level */
#define SEGMENT_RPL_MASK	0x3
/* Bit 2 is table indicator (LDT/GDT) */
#define SEGMENT_TI_MASK		0x4

#define IDT_ENTRIES 256
#define NUM_EXCEPTION_VECTORS 32
/* Bitmask of exception vectors which push an error code on the stack */
#define EXCEPTION_ERRCODE_MASK  0x00027d00
#define GDT_SIZE (GDT_ENTRIES * 8)
#define GDT_ENTRY_TLS_ENTRIES 3
#define TLS_SIZE (GDT_ENTRY_TLS_ENTRIES * 8)

#endif /* _SANDIX_X86_SEGMENT_H */
