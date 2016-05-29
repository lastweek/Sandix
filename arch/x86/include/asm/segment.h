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

/*
 * This file describes x86 segment *layout* in Sandix.
 * See <asm/descriptor.h> for segment descriptor information.
 *
 * There is NO mode bit to disable segmentation. We construct
 * Flat Mode GDT and IDT tables to *bypass* segmentation.
 * See Intel Software Developer Manual Volume 3 for details.
 */

#ifndef _ASM_X86_SEGMENT_H_
#define _ASM_X86_SEGMENT_H_

#include <sandix/const.h>

#define GDT_ENTRY(flags, base, limit)			\
	((((base)  & _AC(0xff000000,ULL)) << 32) |	\
	 (((flags) & _AC(0x0000f0ff,ULL)) << 40) |	\
	 (((limit) & _AC(0x000f0000,ULL)) << 32) |	\
	 (((base)  & _AC(0x00ffffff,ULL)) << 16) |	\
	 (((limit) & _AC(0x0000ffff,ULL))))

#define IDT_ENTRY(flags, select, offset)		\
	((((offset) & _AC(0xffff0000,ULL)) << 32) |	\
	 (((flags)  & _AC(0x0000ff80,ULL)) << 32) |	\
	 (((select) & _AC(0xffff0000,ULL)))       |	\
	 (((offset) & _AC(0x0000ffff,ULL))))

/*               GDT FOR BOOT ONLY                     */
/*******************************************************/
#define GDT_ENTRY_BOOT_CS	2
#define GDT_ENTRY_BOOT_DS	3
#define GDT_ENTRY_BOOT_TSS	4

#define __BOOT_CS	(GDT_ENTRY_BOOT_CS << 3)
#define __BOOT_DS	(GDT_ENTRY_BOOT_DS << 3)
#define __BOOT_TSS	(GDT_ENTRY_BOOT_TSS << 3)


/*               GDT IN SANDIX                        
 ***********************************************************
 * The layout of the per-CPU GDT:
 *
 *   0 - null				<==== cacheline 0
 *   1 - reserved
 *   2 - reserved
 *   3 - reserved
 *
 *   4 - unused				<==== cacheline 1
 *   5 - unused
 *
 *  ------- start of TLS (Thread-Local Storage) segments:
 *
 *   6 - TLS segment #1			[ glibc's TLS segment ]
 *   7 - TLS segment #2			[ Wine's %fs Win32 segment ]
 *   8 - TLS segment #3			<==== cacheline 2
 *   9 - reserved
 *  10 - reserved
 *  11 - reserved
 *
 *  ------- start of kernel segments:
 *
 *  12 - kernel code segment		<==== cacheline 3
 *  13 - kernel data segment
 *  14 - user code segment
 *  15 - user data segment
 *  16 - TSS				<==== cacheline 4
 *  17 - LDT
 *
 *  27 - per-cpu			[ offset to per-cpu data area ]
 *  28 - unused
 *  29 - unused
 *  30 - unused
 *  31 - unused
 ***********************************************************
 */

#define GDT_ENTRY_TLS_ENTRIES		3
#define GDT_ENTRY_TLS_MIN		6
#define GDT_ENTRY_TLS_MAX		8
#define GDT_ENTRY_KERNEL_CS		12
#define GDT_ENTRY_KERNEL_DS		13
#define GDT_ENTRY_USER_CS		14
#define GDT_ENTRY_USER_DS		15
#define GDT_ENTRY_KERNEL_TSS		16
#define GDT_ENTRY_KERNEL_LDT		17
#define GDT_ENTRY_KERNEL_PERCPU		27
#define GDT_ENTRIES			32

#define GDT_SIZE			(GDT_ENTRIES << 3)
#define TLS_SIZE			(GDT_ENTRY_TLS_ENTRIES << 3)

/*
 * Segment selectors (16 bits) layout:
 * Bit 1-0	Ring privilege level
 * Bit 2	Table indicator (LDT or GDT)
 * Bit 15-3	Index into table
 *
 * Segment selectors including: CS, DS, SS, ES, FS, GS.
 */

#define __KERNEL_CS			(GDT_ENTRY_KERNEL_CS << 3)
#define __KERNEL_DS			(GDT_ENTRY_KERNEL_DS << 3)
#define __USER_DS			(GDT_ENTRY_USER_DS << 3)
#define __USER_CS			(GDT_ENTRY_USER_CS << 3)
#define __KERNEL_TSS			(GDT_ENTRY_TSS << 3)
#define __KERNEL_LDT			(GDT_ENTRY_LDT << 3)
#define __KERNEL_PERCPU			(GDT_ENTRY_KERNEL_PERCPU << 3)

#define SEGMENT_RPL_MASK		0x3
#define SEGMENT_TI_MASK			0x4

#define USER_RPL			0x3
#define KERNEL_RPL			0x0

/* LDT segment has TI set, GDT has it cleared */
#define SEGMENT_LDT			0x4
#define SEGMENT_GDT			0x0

#define IDT_ENTRIES 			256
#define NUM_EXCEPTION_VECTORS		32

/* Bitmask of exception vectors which push an error code on the stack */
#define EXCEPTION_ERRCODE_MASK  	0x00027d00

#endif /* _ASM_X86_SEGMENT_H_ */
