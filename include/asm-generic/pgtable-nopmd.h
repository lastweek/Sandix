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

#ifndef _ASM_GENERIC_PGTABLE_NOPMD_H_
#define _ASM_GENERIC_PGTABLE_NOPMD_H_

#ifndef __ASSEMBLY__

#define __PGTABLE_PMD_FOLDED

/*
 * Having the PMD type consist of a PUD gets the size right, and allows
 * us to conceptually access the PUD entry that this PMD is folded into
 * without casting.
 */
typedef struct {
	pud_t pud;
} pmd_t;

#define PMD_SHIFT	PUD_SHIFT
#define PTRS_PER_PMD	1
#define PMD_SIZE	(1UL << PMD_SHIFT)
#define PMD_MASK	(~(PMD_SIZE - 1))

/*
 * The "pud_xxx()" functions here are trivial for a folded two-level
 * setup: the PMD is never bad, and a PMD always exists (as it's folded
 * into the PUD entry)
 */
static inline int pud_none(pud_t pud)		{ return 0; }
static inline int pud_bad(pud_t pud)		{ return 0; }
static inline int pud_present(pud_t pud)	{ return 1; }
static inline void pud_clear(pud_t *pud)	{ }

#define pmd_ERROR(pmd)				(pud_ERROR((pmd).pud))

#define pud_populate(mm, pmd, pte)		do { } while (0)

/*
 * PMDs are folded into PUDs so this doesn't get actually called,
 * but the define is needed for a generic inline function.
 */
#define pud_set(pudptr, pudval) \
	pmd_set((pmd_t *)(pudptr), (pmd_t) { pudval })

static inline pmd_t *pmd_offset(pud_t *pud, unsigned long address)
{
	return (pmd_t *)pud;
}

#define pmd_val(x)				(pud_val((x).pud))
#define __pmd(x)				((pmd_t) { __pud(x) } )

#define pud_page(pud)				(pmd_page((pmd_t){ pud }))
#define pud_page_vaddr(pud)			(pmd_page_vaddr((pmd_t){ pud }))

/*
 * Allocating and freeing a PMD is trivial: the 1-entry PMD is
 * inside the PUD, so has no extra memory associated with it.
 */
#define pmd_alloc_one(mm, address)		NULL
#define pmd_free(mm, x)				do { } while (0)
#define __pmd_free_tlb(tlb, x, a)		do { } while (0)

/* Used to walk through page table */
#undef  pmd_addr_end
#define pmd_addr_end(addr, end)			(end)

#endif /* __ASSEMBLY__ */
#endif /* _ASM_GENERIC_PGTABLE_NOPMD_H_ */
