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

#ifndef _ASM_X86_PGTABLE_TYPES_H_
#define _ASM_X86_PGTABLE_TYPES_H_

/*
 * Bit layout of different page tables.
 */

#define __PAGE_BIT_PRESENT		0	/* is present */
#define __PAGE_BIT_RW			1	/* writeable */
#define __PAGE_BIT_USER			2	/* user addressable */
#define __PAGE_BIT_PWT			3	/* page-level write-through */
#define __PAGE_BIT_PCD			4	/* page-level cache disable */
#define __PAGE_BIT_ACCESSED		5	/* was accessed (raised by CPU) */
#define __PAGE_BIT_DIRTY		6	/* was written to (raied by CPU) */
#define __PAGE_BIT_PSE			7	/* 4 MB (or 2 MB) page */
#define __PAGE_BIT_PAT			7	/* on 4 KB pages */
#define __PAGE_BIT_GLOBAL		8	/* global TLB entry PPro+ */
#define __PAGE_BIT_SOFTW1		9	/* ignored, available for programmer */
#define __PAGE_BIT_SOFTW2		10
#define __PAGE_BIT_SOFTW3		11
#define __PAGE_BIT_PAT_LARGE		12	/* on 2 MB or 1 GB pages */
#define __PAGE_BIT_NX			63	/* execute-disable */

/*
 * If __PAGE_BIT_PRESENT is clear, we use these:
 *  - if the user mapped it with PROT_NONE; pte_present gives true
 */
#define __PAGE_BIT_PROTNONE		__PAGE_BIT_GLOBAL
#define __PAGE_PROTNONE			(_AT(pteval_t, 1) << __PAGE_BIT_PROTNONE)

#define __PAGE_PRESENT			(_AT(pteval_t, 1) << __PAGE_BIT_PRESENT)
#define __PAGE_RW			(_AT(pteval_t, 1) << __PAGE_BIT_RW)
#define __PAGE_USER			(_AT(pteval_t, 1) << __PAGE_BIT_USER)
#define __PAGE_PWT			(_AT(pteval_t, 1) << __PAGE_BIT_PWT)
#define __PAGE_PCD			(_AT(pteval_t, 1) << __PAGE_BIT_PCD)
#define __PAGE_ACCESSED			(_AT(pteval_t, 1) << __PAGE_BIT_ACCESSED)
#define __PAGE_DIRTY			(_AT(pteval_t, 1) << __PAGE_BIT_DIRTY)
#define __PAGE_PSE			(_AT(pteval_t, 1) << __PAGE_BIT_PSE)
#define __PAGE_PAT			(_AT(pteval_t, 1) << __PAGE_BIT_PAT)
#define __PAGE_PAT_LARGE		(_AT(pteval_t, 1) << __PAGE_BIT_PAT_LARGE)
#define __PAGE_GLOBAL			(_AT(pteval_t, 1) << __PAGE_BIT_GLOBAL)
#define __PAGE_SOFTW1			(_AT(pteval_t, 1) << __PAGE_BIT_SOFTW1)
#define __PAGE_SOFTW2			(_AT(pteval_t, 1) << __PAGE_BIT_SOFTW2)
#define __PAGE_SOFTW3			(_AT(pteval_t, 1) << __PAGE_BIT_SOFTW3)

#if defined(CONFIG_X86_64) || defined(CONFIG_X86_PAE)
#define __PAGE_NX			(_AT(pteval_t,1) << __PAGE_BIT_NX)
#else
#define __PAGE_NX			(_AT(pteval_t,0))
#endif

#define __KERNEL_PAGE_TABLE		(__PAGE_PRESENT | __PAGE_RW | __PAGE_ACCESSED | __PAGE_DIRTY)
#define __PAGE_TABLE			(__KERNEL_PAGE_TABLE | __PAGE_USER)

/* set of bits not changed in pte_modify */
#define __PAGE_CHG_MASK			(PTE_PFN_MASK | __PAGE_PCD | __PAGE_PWT | __PAGE_ACCESSED | __PAGE_DIRTY)

#define PAGE_NONE			__pgprot(__PAGE_PROTNONE | __PAGE_ACCESSED)
#define PAGE_SHARED			__pgprot(__PAGE_PRESENT | __PAGE_RW | __PAGE_USER | __PAGE_ACCESSED | __PAGE_NX)
#define PAGE_SHARED_EXEC		__pgprot(__PAGE_PRESENT | __PAGE_RW | __PAGE_USER | __PAGE_ACCESSED)
#define PAGE_READONLY			__pgprot(__PAGE_PRESENT | __PAGE_USER | __PAGE_ACCESSED | __PAGE_NX)
#define PAGE_READONLY_EXEC		__pgprot(__PAGE_PRESENT | __PAGE_USER | __PAGE_ACCESSED)

#define __PAGE_KERNEL_EXEC		(__PAGE_PRESENT | __PAGE_RW | __PAGE_DIRTY | __PAGE_ACCESSED | __PAGE_GLOBAL)
#define __PAGE_KERNEL			(__PAGE_KERNEL_EXEC | __PAGE_NX)
#define __PAGE_KERNEL_RO		(__PAGE_KERNEL & ~__PAGE_RW)
#define __PAGE_KERNEL_RX		(__PAGE_KERNEL_EXEC & ~__PAGE_RW)
#define __PAGE_KERNEL_IO		(__PAGE_KERNEL)

#define PAGE_KERNEL			__pgprot(__PAGE_KERNEL)
#define PAGE_KERNEL_RO			__pgprot(__PAGE_KERNEL_RO)
#define PAGE_KERNEL_EXEC		__pgprot(__PAGE_KERNEL_EXEC)
#define PAGE_KERNEL_RX			__pgprot(__PAGE_KERNEL_RX)
#define PAGE_KERNEL_IO			__pgprot(__PAGE_KERNEL_IO)

#ifdef CONFIG_X86_32
#define PTE_IDENT_ATTR	 0x003		/* PRESENT+RW */
#define PDE_IDENT_ATTR	 0x063		/* PRESENT+RW+DIRTY+ACCESSED */
#define PGD_IDENT_ATTR	 0x001		/* PRESENT (no other attributes) */
#endif

#ifdef CONFIG_X86_32
# include <asm/pgtable_32_types.h>
#else
# include <asm/pgtable_64_types.h>
#endif

/*
 * Extracts the PFN from a (pte|pmd|pud|pgd)val_t of a 4KB page
 * Extracts the flags from a (pte|pmd|pud|pgd)val_t of a 4KB page
 */
#define PTE_PFN_MASK		((pteval_t)PHYSICAL_PAGE_MASK)
#define PTE_FLAGS_MASK		(~PTE_PFN_MASK)

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

static inline pudval_t pud_pfn_mask(pud_t pud)
{
	if (native_pud_val(pud) & __PAGE_PSE)
		return PHYSICAL_PUD_PAGE_MASK;
	else
		return PTE_PFN_MASK;
}

static inline pudval_t pud_flags_mask(pud_t pud)
{
	return ~pud_pfn_mask(pud);
}

static inline pudval_t pud_flags(pud_t pud)
{
	return native_pud_val(pud) & pud_flags_mask(pud);
}

static inline pmdval_t pmd_pfn_mask(pmd_t pmd)
{
	if (native_pmd_val(pmd) & __PAGE_PSE)
		return PHYSICAL_PMD_PAGE_MASK;
	else
		return PTE_PFN_MASK;
}

static inline pmdval_t pmd_flags_mask(pmd_t pmd)
{
	return ~pmd_pfn_mask(pmd);
}

static inline pmdval_t pmd_flags(pmd_t pmd)
{
	return native_pmd_val(pmd) & pmd_flags_mask(pmd);
}

static inline pte_t native_make_pte(pteval_t val)
{
	return (pte_t) { .pte = val };
}

static inline pteval_t native_pte_val(pte_t pte)
{
	return pte.pte;
}

static inline pteval_t pte_flags(pte_t pte)
{
	return native_pte_val(pte) & PTE_FLAGS_MASK;
}

#define pgprot_val(x)	((x).pgprot)
#define __pgprot(x)	( (pgprot_t) { (x) } )

#endif /* _ASM_X86_PGTABLE_TYPES_H_ */
