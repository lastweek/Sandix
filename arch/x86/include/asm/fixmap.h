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

#ifndef _ASM_X86_FIXMAP_H_
#define _ASM_X86_FIXMAP_H_

#include <asm/page.h>
#include <asm/apic.h>
#include <asm/pgtable.h>

#include <sandix/bug.h>
#include <sandix/threads.h>

/*
 * Compile-time virtual memory allocation
 *
 * Here we define all the compile-time *special* virtual addresses.
 * The point is to have a constant address at compile time, but to
 * set the physical address only in the boot process.
 *
 * For x86_32, we allocate these special addresses from the end of
 * virtual memory (0xfffff000) backwards. Also, this lets us do
 * fail-safe vmalloc(), we can guarantee that these special
 * addresses and vmalloc()-ed addresses never overlap.
 *
 * These compile-time allocated memory buffers are fixed 4KB pages.
 * Use set_fixmap(idx, phys) to associate physical memory with fixmap
 * indices.
 *
 * TLB entries of such buffers will not be flushed across task switches.
 */

#ifdef CONFIG_X86_32
extern unsigned long 	__FIXADDR_TOP;
#define FIXADDR_TOP	((unsigned long)__FIXADDR_TOP)
#else
#define FIXADDR_TOP	BUILD_BUG_ON(1)
#endif

enum fixed_addresses {

#ifdef CONFIG_X86_32
	FIX_HOLE,
#else
	FIX_VSYSCALL_PAGE,
#endif

#ifdef CONFIG_X86_LOCAL_APIC
	FIX_APIC_BASE,
#endif

#ifdef CONFIG_X86_IO_APIC
	FIX_IO_APIC_BASE_0,
	FIX_IO_APIC_BASE_END = FIX_IO_APIC_BASE_0 + MAX_IO_APICS - 1,
#endif

	/*
	 * Reserved pte's for temporary kernel mappings
	 */
#define KM_TYPE_NR		20
#ifdef CONFIG_X86_32
	FIX_KMAP_BEGIN,
	FIX_KMAP_END = FIX_KMAP_BEGIN + (KM_TYPE_NR * NR_CPUS) - 1,
#ifdef CONFIG_PCI_MMCONFIG
	FIX_PCIE_MCFG,
#endif
#endif

	__end_of_permanent_fixed_addresses,

	/*
	 * 512 temporary boot-time mappings, used by
	 * early_ioremap(), before ioremap() is functional.
	 *
	 * If necessary we round it up to the next 512 pages boundary so
	 * that we can have a single pgd entry and a single pte table:
	 */
#define NR_FIX_BTMAPS		64
#define FIX_BTMAPS_SLOTS	8
#define TOTAL_FIX_BTMAPS	(NR_FIX_BTMAPS * FIX_BTMAPS_SLOTS)

	FIX_BTMAP_END =
	 (__end_of_permanent_fixed_addresses ^
	  (__end_of_permanent_fixed_addresses + TOTAL_FIX_BTMAPS - 1)) &
	 -PTRS_PER_PTE
	 ? __end_of_permanent_fixed_addresses + TOTAL_FIX_BTMAPS -
	   (__end_of_permanent_fixed_addresses & (TOTAL_FIX_BTMAPS - 1))
	 : __end_of_permanent_fixed_addresses,
	FIX_BTMAP_BEGIN = FIX_BTMAP_END + TOTAL_FIX_BTMAPS - 1,

	__end_of_fixed_addresses
};

#define FIXADDR_SIZE	(__end_of_permanent_fixed_addresses << PAGE_SHIFT)
#define FIXADDR_START	(FIXADDR_TOP - FIXADDR_SIZE)

void __set_fixmap(enum fixed_addresses idx, phys_addr_t phys, pgprot_t flags);

void __early_set_fixmap(enum fixed_addresses idx, phys_addr_t phys, pgprot_t flags);

#define __late_set_fixmap(idx, phys, flags) __set_fixmap(idx, phys, flags)
#define __late_clear_fixmap(idx) __set_fixmap(idx, 0, __pgprot(0))

#include <asm-generic/fixmap.h>

#endif /* _ASM_X86_FIXMAP_H_ */
