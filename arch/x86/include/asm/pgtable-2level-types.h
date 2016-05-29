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

#ifndef _ASM_X86_PGTABLE_2LEVEL_TYPES_H_
#define _ASM_X86_PGTABLE_2LEVEL_TYPES_H_

/*
 * Traditional i386 two-level paging structure
 */

#ifndef __ASSEMBLY__
typedef unsigned long pteval_t;
typedef unsigned long pmdval_t;
typedef unsigned long pudval_t;
typedef unsigned long pgdval_t;
typedef unsigned long pgprotval_t;

typedef union {
	pteval_t pte;
	pteval_t pte_low;
} pte_t;
#endif

/*
 * PGDIR_SHIFT determines the size of the area
 * a top-level page table entry can map.
 */
#define PGDIR_SHIFT		22
#define PTRS_PER_PGD		1024

/*
 * The i386 is two-level, so we do not really
 * have any PUD and PMD directory physically.
 */

/*
 * Entries per page directory level
 */
#define PTRS_PER_PTE		1024

#endif /* _ASM_X86_PGTABLE_2LEVEL_TYPES_H_ */
