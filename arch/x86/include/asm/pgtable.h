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
 * There are 10 header files about page table (wtf?). Depends on the
 * configuration, only a subset of them will be used in a certain build.
 * Now, let me draw the whole picture first...
 *
 * Basic page table types:
 *
 *	pgtable_types.h
 *		+ generic types
 *		+ pgtable_32_types.h
 * or
 *	pgtable_types.h
 *		+ generic types
 *		+ pgtable_64_types.h
 *
 * The final page table header file:
 *
 *	pgtable.h
 *		+ pgtable_types.h
 *		+ pgtable_32.h
 *		+ generic ops
 * or
 *	pgtbale.h
 *		+ pgtable_types.h
 *		+ pgtable_64.h
 *		+ generic ops
 *
 * Especially, depends on whether PAE is enabled, i386
 * has two mapping modes:
 *
 *	pgtable_32.h
 *		+ pgtable-2level.h
 *	pgtable_32_types.h
 *		+ pgtable-2level-types.h
 * or
 *	pgtbale_32.h
 *		+ pgtable-3level.h
 *	pgtable_32_types.h
 *		+ pgtable-3level-types.h
 */

#ifndef _ASM_X86_PGTABLE_H_
#define _ASM_X86_PGTABLE_H_

#include <sandix/const.h>
#include <sandix/types.h>

#include <asm/page.h>
#include <asm/pgtable_types.h>

/* Skip paravirt */

/*
 * Batch 1
 */

#ifndef __PGTABLE_PUD_FOLDED
#define pgd_set(pgdp, pgd)		native_pgd_set(pgdp, pgd)
#define pgd_clear(pgd)			native_pgd_clear(pgd)
#endif

#ifndef __PGTABLE_PMD_FOLDED
#define pud_set(pudp, pud)		native_pud_set(pudp, pud)
#define pud_clear(pud)			native_pud_clear(pud)
#endif

#define pmd_set(pmdp, pmd)		native_pmd_set(pmdp, pmd)
#define pmd_clear(pmd)			native_pmd_clear(pmd)

#define pte_set(ptep, pte)		native_pte_set(ptep, pte)
#define pte_clear(addr, ptep)		native_pte_clear(addr, ptep)

/*
 * Batch 2
 */

#define pgd_val(x)			native_pgd_val(x)
#define __pgd(x)			native_make_pgd(x)

#ifndef __PGTABLE_PUD_FOLDED
#define pud_val(x)			native_pud_val(x)
#define __pud(x)			native_make_pud(x)
#endif

#ifndef __PGTABLE_PMD_FOLDED
#define pmd_val(x)			native_pmd_val(x)
#define __pmd(x)			native_make_pmd(x)
#endif

#define pte_val(x)			native_pte_val(x)
#define __pte(x)			native_make_pte(x)

/*
 * Batch 3
 */

static inline int pte_none(pte_t pte)
{
	return !pte.pte;
}

#define __HAVE_ARCH_PTE_SAME
static inline int pte_same(pte_t a, pte_t b)
{
	return a.pte == b.pte;
}

static inline int pte_present(pte_t a)
{
	return pte_flags(a) & __PAGE_PSE;
}

static inline int pte_dirty(pte_t pte)
{
	return pte_flags(pte) & __PAGE_DIRTY;
}

static inline int pte_accessed(pte_t pte)
{
	return pte_flags(pte) & __PAGE_ACCESSED;
}

static inline int pte_write(pte_t pte)
{
	return pte_flags(pte) & __PAGE_RW;
}

static inline int pte_huge(pte_t pte)
{
	return pte_flags(pte) & __PAGE_PSE;
}

static inline int pte_global(pte_t pte)
{
	return pte_flags(pte) & __PAGE_GLOBAL;
}

static inline int pte_exec(pte_t pte)
{
	return !(pte_flags(pte) & __PAGE_NX);
}

static inline int pmd_dirty(pmd_t pmd)
{
	return pmd_flags(pmd) & __PAGE_DIRTY;
}

static inline int pmd_accessed(pmd_t pmd)
{
	return pmd_flags(pmd) & __PAGE_ACCESSED;
}

/* Get Page Frame Number */
static inline unsigned long pte_pfn(pte_t pte)
{
	return (pte_val(pte) & PTE_PFN_MASK) >> PAGE_SHIFT;
}

static inline unsigned long pmd_pfn(pmd_t pmd)
{
	return (pmd_val(pmd) & pmd_pfn_mask(pmd)) >> PAGE_SHIFT;
}

static inline unsigned long pud_pfn(pud_t pud)
{
	return (pud_val(pud) & pud_pfn_mask(pud)) >> PAGE_SHIFT;
}

/*
 * Batch 4
 */

static inline pte_t pte_set_flags(pte_t pte, pteval_t set)
{
	pteval_t v = pte_val(pte);

	return __pte(v | set);
}

static inline pte_t pte_clear_flags(pte_t pte, pteval_t clear)
{
	pteval_t v = pte_val(pte);

	return __pte(v & ~clear);
}

static inline pte_t pte_mkclean(pte_t pte)
{
	return pte_clear_flags(pte, __PAGE_DIRTY);
}

static inline pte_t pte_mkdirty(pte_t pte)
{
	return pte_set_flags(pte, __PAGE_DIRTY);
}

static inline pte_t pte_mkexec(pte_t pte)
{
	return pte_clear_flags(pte, __PAGE_NX);
}

static inline pte_t pte_mkold(pte_t pte)
{
	return pte_clear_flags(pte, __PAGE_ACCESSED);
}

static inline pte_t pte_mkyoung(pte_t pte)
{
	return pte_set_flags(pte, __PAGE_ACCESSED);
}

#ifdef CONFIG_X86_32
#include <asm/pgtable_32.h>
#else
#include <asm/pgtable_64.h>
#endif














#endif /* _ASM_X86_PGTABLE_H_ */
