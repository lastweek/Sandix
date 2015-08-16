/*
 *	x86 Two-Level Paging Mapping
 *
 *	Copyright (C) 2015 Yizhou Shan <shanyizhou@ict.ac.cn>
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

#ifndef _ASM_PGTABLE_H_
#define _ASM_PGTABLE_H_

#include <sandix/const.h>
#include <sandix/types.h>
#include <asm/page.h>

typedef unsigned long	pteval_t;
typedef unsigned long	pdeval_t;

typedef struct {
	pteval_t	pte;
	pteval_t	pte_low;
} pte_t;

typedef struct {
	pdeval_t	pde;
	pdeval_t	pde_low;
} pde_t;

/* Two-Level Paing Parameters */
#define PGDIR_SHIFT		22
#define PTRS_PER_PGD		1024
#define PTRS_PER_PTE		1024

#define __PAGE_BIT_PRESENT	0	/* is present */
#define __PAGE_BIT_RW		1	/* is writeable */
#define __PAGE_BIT_USER		2	/* is user addressable */
#define __PAGE_BIT_PWT		3	/* page-level write-through */
#define __PAGE_BIT_PCD		4	/* page-level cache disable */
#define __PAGE_BIT_ACCESSED	5	/* was accessed (raised by CPU) */
#define __PAGE_BIT_DIRTY	6	/* was written to (raied by CPU) */
#define __PAGE_BIT_PSE		7
#define __PAGE_BIT_PAT		7
#define __PAGE_BIT_GLOBAL	8	/* Global TLB entry PPro+ */

#define __PAGE_RRESENT		(_AT(pteval_t,1) << __PAGE_BIT_PRESENT)
#define __PAGE_RW		(_AT(pteval_t,1) << __PAGE_BIT_RW)
#define __PAGE_USER		(_AT(pteval_t,1) << __PAGE_BIT_USER)
#define __PAGE_PWT		(_AT(pteval_t,1) << __PAGE_BIT_PWT)
#define __PAGE_PCD		(_AT(pteval_t,1) << __PAGE_BIT_PCD)
#define __PAGE_ACCESSED		(_AT(pteval_t,1) << __PAGE_BIT_ACCESSED)
#define __PAGE_DIRTY		(_AT(pteval_t,1) << __PAGE_BIT_DIRTY)
#define __PAGE_PSE		(_AT(pteval_t,1) << __PAGE_BIT_PSE)
#define __PAGE_PAT		(_AT(pteval_t,1) << __PAGE_BIT_PAT)
#define __PAGE_GLOBAL		(_AT(pteval_t,1) << __PAGE_BIT_GLOBAL)

static inline void set_pte(pte_t pteval, pte_t *ptep)
{
	*ptep = pteval;
}

static inline void set_pde(pde_t pdeval, pde_t *pdep)
{
	*pdep = pdeval;
}


#endif /* _ASM_PGTABLE_H_ */
