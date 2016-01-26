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
 * Consult Intel SDM-Vol 3, Chapter 4: Paging for details.
 */

#ifndef _ASM_X86_PGTABLE_TYPES_H_
#define _ASM_X86_PGTABLE_TYPES_H_

#ifndef _ASM_X86_PGTABLE_H_
# error "Please do not include this file directly"
#endif

#include <sandix/const.h>
#include <sandix/types.h>

/*
 * Bit layout of different layer page tables.
 */

#define __PAGE_BIT_PRESENT	0	/* is present */
#define __PAGE_BIT_RW		1	/* writeable */
#define __PAGE_BIT_USER		2	/* user addressable */
#define __PAGE_BIT_PWT		3	/* page-level write-through */
#define __PAGE_BIT_PCD		4	/* page-level cache disable */
#define __PAGE_BIT_ACCESSED	5	/* was accessed (raised by CPU) */
#define __PAGE_BIT_DIRTY	6	/* was written to (raied by CPU) */
#define __PAGE_BIT_PSE		7	/* 4 MB (or 2 MB) page */
#define __PAGE_BIT_PAT		7	/* on 4 KB pages */
#define __PAGE_BIT_GLOBAL	8	/* global TLB entry PPro+ */
#define __PAGE_BIT_SOFTW1	9	/* ignored, available for programmer */
#define __PAGE_BIT_SOFTW2	10
#define __PAGE_BIT_SOFTW3	11
#define __PAGE_BIT_PAT_LARGE	12	/* on 2 MB or 1 GB pages */
#define __PAGE_BIT_NX		63	/* execute-disable */

#define __PAGE_RRESENT		(_AT(pteval_t,1) << __PAGE_BIT_PRESENT)
#define __PAGE_RW		(_AT(pteval_t,1) << __PAGE_BIT_RW)
#define __PAGE_USER		(_AT(pteval_t,1) << __PAGE_BIT_USER)
#define __PAGE_PWT		(_AT(pteval_t,1) << __PAGE_BIT_PWT)
#define __PAGE_PCD		(_AT(pteval_t,1) << __PAGE_BIT_PCD)
#define __PAGE_ACCESSED		(_AT(pteval_t,1) << __PAGE_BIT_ACCESSED)
#define __PAGE_DIRTY		(_AT(pteval_t,1) << __PAGE_BIT_DIRTY)
#define __PAGE_PSE		(_AT(pteval_t,1) << __PAGE_BIT_PSE)
#define __PAGE_PAT		(_AT(pteval_t,1) << __PAGE_BIT_PAT)
#define __PAGE_PAT_LARGE	(_AT(pteval_t,1) << __PAGE_BIT_PAT_LARGE)
#define __PAGE_GLOBAL		(_AT(pteval_t,1) << __PAGE_BIT_GLOBAL)
#define __PAGE_SOFTW1		(_AT(pteval_t,1) << __PAGE_BIT_SOFTW1)
#define __PAGE_SOFTW2		(_AT(pteval_t,1) << __PAGE_BIT_SOFTW2)
#define __PAGE_SOFTW3		(_AT(pteval_t,1) << __PAGE_BIT_SOFTW3)

#if defined(CONFIG_X86_64) || defined(CONFIG_X86_PAE)
#define __PAGE_NX		(_AT(pteval_t,1) << __PAGE_BIT_NX)
#else
#define __PAGE_NX		(_AT(pteval_t,0))
#endif

#ifndef __ASSEMBLY__

#ifdef CONFIG_X86_32
# include <asm/pgtable_32_types.h>
#else
# include <asm/pgtable_64_types.h>
#endif

//TODO
#define PTE_FLAGS_MASK	1

typedef struct {
	pgdval_t pgd;
} pgd_t;

typedef struct {
	pgprotval_t pgprot;
} pgprot_t;

static inline pgd_t native_make_pgd(pgdval_t val)
{
	return (pgd_t) { val };
}

static inline pgdval_t native_pgd_val(pgd_t pgd)
{
	return pgd.pgd;
}

static inline pgdval_t pgd_flags(pgd_t pgd)
{
	return native_pgd_val(pgd) & PTE_FLAGS_MASK;
}

#if CONFIG_PGTABLE_LEVELS > 3
typedef struct {
	pudval_t pud;
} pud_t;

/*XXX pmd?? */
static inline pud_t native_make_pud(pmdval_t val)
{
	return (pud_t) { val };
}

static inline pudval_t native_pud_val(pud_t pud)
{
	return pud.pud;
}
#else
#include <asm-generic/pgtable-nopud.h>

static inline pudval_t native_pud_val(pud_t pud)
{
/*XXX pgd?*/
	return native_pgd_val(pud.pgd);
}
#endif

#if CONFIG_PGTABLE_LEVELS > 2
typedef struct {
	pmdval_t pmd;
} pmd_t;

static inline pmd_t native_make_pmd(pmdval_t val)
{
	return (pmd_t) { val };
}

static inline pmdval_t native_pmd_val(pmd_t pmd)
{
	return pmd.pmd;
}
#else
#include <asm-generic/pgtable-nopmd.h>

static inline pmdval_t native_pmd_val(pmd_t pmd)
{
	return native_pgd_val(pmd.pud.pgd);
}
#endif

#endif /* __ASSEMBLY__ */

#endif /* _ASM_X86_PGTABLE_TYPES_H_ */
