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
 * Three-level paging is enabled when PAE is enabled in 32-bit x86.
 * PAE paging translates 32-bit linear address to 52-bit physical
 * address. If MAXPHYADDR < 52, bits in the range 51:MAXPHYADDR
 * will be 0 in any physical address used by PAE paging. The
 * MAXPHYADDR depends on each processor. Normally, 36-bit(?).
 */

#ifndef _ASM_X86_PGTABLE_3LEVEL_TYPES_H_
#define _ASM_X86_PGTABLE_3LEVEL_TYPES_H_

#ifndef __ASSEMBLY__
#include <sandix/types.h>

typedef u64 pteval_t;
typedef u64 pmdval_t;
typedef u64 pudval_t;
typedef u64 pgdval_t;
typedef u64 pgprotval_t;

typedef union {
	struct {
		unsigned long pte_low, pte_high;
	};
	pteval_t pte;
} pte_t;
#endif /* __ASSEMBLY__ */

#define SHARED_KERNEL_PMD	1

/*
 * PGDIR_SHIFT determines what a top-level page table entry can map
 */
#define PGDIR_SHIFT		30
#define PTRS_PER_PGD		4

/*
 * PMD_SHIFT determines the size of the area a middle-level page table can map
 */
#define PMD_SHIFT		21
#define PTRS_PER_PMD		512

/*
 * Entries per page directory level
 */
#define PTRS_PER_PTE		512

#endif /* _ASM_X86_PGTABLE_3LEVEL_TYPES_H_ */

