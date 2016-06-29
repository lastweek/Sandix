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
#define __PAGE_BIT_SOFTW1		9	/* available for programmer */
#define __PAGE_BIT_SOFTW2		10	/* */
#define __PAGE_BIT_SOFTW3		11	/* */
#define __PAGE_BIT_PAT_LARGE		12	/* on 2 MB or 1 GB pages */
#define __PAGE_BIT_SOFTW4		58	/* available for programmer */
#define __PAGE_BIT_PKEY_BIT0		59	/* Protection Keys, bit 1/4 */
#define __PAGE_BIT_PKEY_BIT1		60	/* Protection Keys, bit 2/4 */
#define __PAGE_BIT_PKEY_BIT2		61	/* Protection Keys, bit 3/4 */
#define __PAGE_BIT_PKEY_BIT3		62	/* Protection Keys, bit 4/4 */
#define __PAGE_BIT_NX			63	/* execute-disable */

#define __PAGE_BIT_SPECIAL		__PAGE_BIT_SOFTW1
#define __PAGE_BIT_CPA_TEST		__PAGE_BIT_SOFTW1
#define __PAGE_BIT_HIDDEN		__PAGE_BIT_SOFTW3 /* hidden by kmemcheck */
#define __PAGE_BIT_SOFT_DIRTY		__PAGE_BIT_SOFTW3 /* software dirty tracking */

/*
 * If __PAGE_BIT_PRESENT is clear, we use these:
 *  - if the user mapped it with PROT_NONE; pte_present gives true
 */
#define __PAGE_BIT_PROTNONE		__PAGE_BIT_GLOBAL

#define __PAGE_PRESENT			(_AT(pteval_t, 1) << __PAGE_BIT_PRESENT)
#define __PAGE_RW			(_AT(pteval_t, 1) << __PAGE_BIT_RW)
#define __PAGE_USER			(_AT(pteval_t, 1) << __PAGE_BIT_USER)
#define __PAGE_PWT			(_AT(pteval_t, 1) << __PAGE_BIT_PWT)
#define __PAGE_PCD			(_AT(pteval_t, 1) << __PAGE_BIT_PCD)
#define __PAGE_ACCESSED			(_AT(pteval_t, 1) << __PAGE_BIT_ACCESSED)
#define __PAGE_DIRTY			(_AT(pteval_t, 1) << __PAGE_BIT_DIRTY)
#define __PAGE_PSE			(_AT(pteval_t, 1) << __PAGE_BIT_PSE)
#define __PAGE_GLOBAL			(_AT(pteval_t, 1) << __PAGE_BIT_GLOBAL)
#define __PAGE_PROTNONE			(_AT(pteval_t, 1) << __PAGE_BIT_PROTNONE)
#define __PAGE_SOFTW1			(_AT(pteval_t, 1) << __PAGE_BIT_SOFTW1)
#define __PAGE_SOFTW2			(_AT(pteval_t, 1) << __PAGE_BIT_SOFTW2)
#define __PAGE_PAT			(_AT(pteval_t, 1) << __PAGE_BIT_PAT)
#define __PAGE_PAT_LARGE		(_AT(pteval_t, 1) << __PAGE_BIT_PAT_LARGE)
#define __PAGE_SOFTW3			(_AT(pteval_t, 1) << __PAGE_BIT_SOFTW3)
#define __PAGE_SPECIAL			(_AT(pteval_t, 1) << __PAGE_BIT_SPECIAL)
#define __PAGE_CPA_TEST			(_AT(pteval_t, 1) << __PAGE_BIT_CPA_TEST)

#ifdef CONFIG_X86_INTEL_MEMORY_PROTECTION_KEYS
#define __PAGE_PKEY_BIT0		(_AT(pteval_t, 1) << __PAGE_BIT_PKEY_BIT0)
#define __PAGE_PKEY_BIT1		(_AT(pteval_t, 1) << __PAGE_BIT_PKEY_BIT1)
#define __PAGE_PKEY_BIT2		(_AT(pteval_t, 1) << __PAGE_BIT_PKEY_BIT2)
#define __PAGE_PKEY_BIT3		(_AT(pteval_t, 1) << __PAGE_BIT_PKEY_BIT3)
#else
#define __PAGE_PKEY_BIT0		(_AT(pteval_t, 0))
#define __PAGE_PKEY_BIT1		(_AT(pteval_t, 0))
#define __PAGE_PKEY_BIT2		(_AT(pteval_t, 0))
#define __PAGE_PKEY_BIT3		(_AT(pteval_t, 0))
#endif
#define __PAGE_PKEY_MASK		(__PAGE_PKEY_BIT0 | \
					 __PAGE_PKEY_BIT1 | \
					 __PAGE_PKEY_BIT2 | \
					 __PAGE_PKEY_BIT3)

#define __HAVE_ARCH_PTE_SPECIAL

#ifdef CONFIG_KMEMCHECK
#define __PAGE_HIDDEN			(_AT(pteval_t, 1) << __PAGE_BIT_HIDDEN)
#else
#define __PAGE_HIDDEN			(_AT(pteval_t, 0))
#endif
/*
 * The same hidden bit is used by kmemcheck, but since kmemcheck
 * works on kernel pages while soft-dirty engine on user space,
 * they do not conflict with each other.
 */
#ifdef CONFIG_MEM_SOFT_DIRTY
#define __PAGE_SOFT_DIRTY		(_AT(pteval_t, 1) << __PAGE_BIT_SOFT_DIRTY)
#else
#define __PAGE_SOFT_DIRTY		(_AT(pteval_t, 0))
#endif

#if defined(CONFIG_X86_64) || defined(CONFIG_X86_PAE)
#define __PAGE_NX			(_AT(pteval_t,1) << __PAGE_BIT_NX)
#else
#define __PAGE_NX			(_AT(pteval_t,0))
#endif

#define __PAGE_TABLE			(__PAGE_PRESENT | __PAGE_RW | __PAGE_USER | \
					 __PAGE_ACCESSED | __PAGE_DIRTY)

#define __KERNEL_TABLE			(__PAGE_PRESENT | __PAGE_RW | __PAGE_ACCESSED | \
					 __PAGE_DIRTY)

/* Set of bits not changed in pte_modify */
#define __PAGE_CHG_MASK			(PTE_PFN_MASK | __PAGE_PCD | __PAGE_PWT | \
					 __PAGE_SPECIAL | __PAGE_ACCESSED | __PAGE_DIRTY | \
					 __PAGE_SOFT_DIRTY)

/*
 * The cache modes defined here are used to translate between pure SW usage
 * and the HW defined cache mode bits and/or PAT entries.
 *
 * The resulting bits for PWT, PCD and PAT should be chosen in a way
 * to have the WB mode at index 0 (all bits clear). This is the default
 * right now and likely would break too much if changed.
 */
#ifndef __ASSEMBLY__
enum page_cache_mode {
	__PAGE_CACHE_MODE_WB		= 0,
	__PAGE_CACHE_MODE_WC		= 1,
	__PAGE_CACHE_MODE_UC_MINUS	= 2,
	__PAGE_CACHE_MODE_UC		= 3,
	__PAGE_CACHE_MODE_WT		= 4,
	__PAGE_CACHE_MODE_WP		= 5,
	__PAGE_CACHE_MODE_NUM		= 8
};
#endif

#define __PAGE_CACHE_MASK		(__PAGE_PAT | __PAGE_PCD | __PAGE_PWT)
#define __PAGE_NOCACHE			(cachemode2protval(__PAGE_CACHE_MODE_UC))

#define PAGE_NONE			__pgprot(__PAGE_PROTNONE | __PAGE_ACCESSED)
#define PAGE_SHARED			__pgprot(__PAGE_PRESENT | __PAGE_RW | __PAGE_USER | \
						 __PAGE_ACCESSED | __PAGE_NX)
#define PAGE_SHARED_EXEC		__pgprot(__PAGE_PRESENT | __PAGE_RW | __PAGE_USER | \
						 __PAGE_ACCESSED)
#define PAGE_COPY_NOEXEC		__pgprot(__PAGE_PRESENT | __PAGE_USER |	\
						 __PAGE_ACCESSED | __PAGE_NX)
#define PAGE_COPY_EXEC			__pgprot(__PAGE_PRESENT | __PAGE_USER |	\
						 __PAGE_ACCESSED)
#define PAGE_COPY			PAGE_COPY_NOEXEC
#define PAGE_READONLY			__pgprot(__PAGE_PRESENT | __PAGE_USER | \
						 __PAGE_ACCESSED | __PAGE_NX)
#define PAGE_READONLY_EXEC		__pgprot(__PAGE_PRESENT | __PAGE_USER | \
						 __PAGE_ACCESSED)

#define __PAGE_KERNEL_EXEC		(__PAGE_PRESENT | __PAGE_RW | __PAGE_DIRTY | \
					 __PAGE_ACCESSED | __PAGE_GLOBAL)
#define __PAGE_KERNEL			(__PAGE_KERNEL_EXEC | __PAGE_NX)
#define __PAGE_KERNEL_RO		(__PAGE_KERNEL & ~__PAGE_RW)
#define __PAGE_KERNEL_RX		(__PAGE_KERNEL_EXEC & ~__PAGE_RW)
#define __PAGE_KERNEL_NOCACHE		(__PAGE_KERNEL | __PAGE_NOCACHE)
#define __PAGE_KERNEL_LARGE		(__PAGE_KERNEL | __PAGE_PSE)
#define __PAGE_KERNEL_LARGE_EXEC	(__PAGE_KERNEL_EXEC | __PAGE_PSE)
#define __PAGE_KERNEL_IO		(__PAGE_KERNEL)
#define __PAGE_KERNEL_IO_NOCACHE	(__PAGE_KERNEL_NOCACHE)

#define PAGE_KERNEL			__pgprot(__PAGE_KERNEL)
#define PAGE_KERNEL_RO			__pgprot(__PAGE_KERNEL_RO)
#define PAGE_KERNEL_EXEC		__pgprot(__PAGE_KERNEL_EXEC)
#define PAGE_KERNEL_NOCACHE		__pgprot(__PAGE_KERNEL_NOCACHE)
#define PAGE_KERNEL_LARGE		__pgprot(__PAGE_KERNEL_LARGE)
#define PAGE_KERNEL_LARGE_EXEC		__pgprot(__PAGE_KERNEL_LARGE_EXEC)
#define PAGE_KERNEL_RX			__pgprot(__PAGE_KERNEL_RX)
#define PAGE_KERNEL_IO			__pgprot(__PAGE_KERNEL_IO)
#define PAGE_KERNEL_IO_NOCACHE		__pgprot(__PAGE_KERNEL_IO_NOCACHE)

/*         xwr */
#define __P000	PAGE_NONE
#define __P001	PAGE_READONLY
#define __P010	PAGE_COPY
#define __P011	PAGE_COPY
#define __P100	PAGE_READONLY_EXEC
#define __P101	PAGE_READONLY_EXEC
#define __P110	PAGE_COPY_EXEC
#define __P111	PAGE_COPY_EXEC

#define __S000	PAGE_NONE
#define __S001	PAGE_READONLY
#define __S010	PAGE_SHARED
#define __S011	PAGE_SHARED
#define __S100	PAGE_READONLY_EXEC
#define __S101	PAGE_READONLY_EXEC
#define __S110	PAGE_SHARED_EXEC
#define __S111	PAGE_SHARED_EXEC

/*
 * Early identity mapping. Used by head.S and early init_mem_mapping
 * to establish identity mapping.
 */
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
 * Extracts the flags from a (pte|pmd|pud|pgd)val_t
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

#define pgprot_val(x)			((x).pgprot)
#define __pgprot(x)			((pgprot_t) { (x) })

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

static inline pte_t native_make_pte(pteval_t val)
{
	return (pte_t) { .pte = val };
}

static inline pteval_t native_pte_val(pte_t pte)
{
	return pte.pte;
}

extern u16 __cachemode2pte_tbl[__PAGE_CACHE_MODE_NUM];
extern u8 __pte2cachemode_tbl[8];

#define __pte2cm_idx(cb)				\
	((((cb) >> (__PAGE_BIT_PAT - 2)) & 4) |		\
	 (((cb) >> (__PAGE_BIT_PCD - 1)) & 2) |		\
	 (((cb) >> __PAGE_BIT_PWT) & 1))
#define __cm_idx2pte(i)					\
	((((i) & 4) << (__PAGE_BIT_PAT - 2)) |		\
	 (((i) & 2) << (__PAGE_BIT_PCD - 1)) |		\
	 (((i) & 1) << __PAGE_BIT_PWT))

static inline unsigned long cachemode2protval(enum page_cache_mode pcm)
{
	if (likely(pcm == 0))
		return 0;
	return __cachemode2pte_tbl[pcm];
}

static inline pgprot_t cachemode2pgprot(enum page_cache_mode pcm)
{
	return __pgprot(cachemode2protval(pcm));
}

static inline enum page_cache_mode pgprot2cachemode(pgprot_t pgprot)
{
	unsigned long masked;

	masked = pgprot_val(pgprot) & __PAGE_CACHE_MASK;
	if (likely(masked == 0))
		return 0;
	return __pte2cachemode_tbl[__pte2cm_idx(masked)];
}

static inline pgprot_t pgprot_4k_2_large(pgprot_t pgprot)
{
	pgprotval_t val = pgprot_val(pgprot);
	pgprot_t new;

	pgprot_val(new) = (val & ~(__PAGE_PAT | __PAGE_PAT_LARGE)) |
		((val & __PAGE_PAT) << (__PAGE_BIT_PAT_LARGE - __PAGE_BIT_PAT));
	return new;
}

static inline pgprot_t pgprot_large_2_4k(pgprot_t pgprot)
{
	pgprotval_t val = pgprot_val(pgprot);
	pgprot_t new;

	pgprot_val(new) = (val & ~(__PAGE_PAT | __PAGE_PAT_LARGE)) |
			  ((val & __PAGE_PAT_LARGE) >>
			   (__PAGE_BIT_PAT_LARGE - __PAGE_BIT_PAT));
	return new;
}

enum pg_level {
	PG_LEVEL_NONE,
	PG_LEVEL_4K,
	PG_LEVEL_2M,
	PG_LEVEL_1G,
	PG_LEVEL_NUM
};

void update_page_count(int level, unsigned long pages);

#endif /* _ASM_X86_PGTABLE_TYPES_H_ */
