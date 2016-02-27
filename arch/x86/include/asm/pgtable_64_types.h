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

#ifndef _ASM_X86_PGTABLE_64_TYPES_H_
#define _ASM_X86_PGTABLE_64_TYPES_H_

#ifndef __ASSEMBLY__
typedef unsigned long pteval_t;
typedef unsigned long pmdval_t;
typedef unsigned long pudval_t;
typedef unsigned long pgdval_t;
typedef unsigned long pgprotval_t;

typedef struct {
	pteval_t pte;
} pte_t;
#endif

/*
 * PGDIR_SHIFT determines the size of the area
 * a top-level page table entry can map.
 */
#define PGDIR_SHIFT		39
#define PTRS_PER_PGD		512

/*
 * PUD_SHIFT determines the size of the area
 * a 3rd-level page table entry can map.
 */
#define PUD_SHIFT		30
#define PTRS_PER_PUD		512

/*
 * PMD_SHIFT determines the size of the area
 * a middle-level page table entry can map.
 */
#define PMD_SHIFT		21
#define PTRS_PER_PMD		512

/*
 * Entries per page directory level
 */
#define PTRS_PER_PTE		512

#define PMD_SIZE	(1UL << PMD_SHIFT)
#define PMD_MASK	(~(PMD_SIZE - 1))
#define PUD_SIZE	(1UL << PUD_SHIFT)
#define PUD_MASK	(~(PUD_SIZE - 1))
#define PGDIR_SIZE	(1UL << PGDIR_SHIFT)
#define PGDIR_MASK	(~(PGDIR_SHIFT - 1))

#endif /* _ASM_X86_PGTABLE_32_TYPES_H_ */
