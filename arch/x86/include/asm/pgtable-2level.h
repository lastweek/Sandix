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

#ifndef _ASM_X86_PGTABLE_2LEVEL_H_
#define _ASM_X86_PGTABLE_2LEVEL_H_

/*
 * Traditional i386 two-level paging structure
 *
 * We fold PMD into PUD to fit with x86_32's 3-level framework.
 */

#ifndef __ASSEMBLY__

#define pte_ERROR(e) \
	pr_err("%s:%d: bad pte %#08lx\n", __FILE__, __LINE__, (e).pte_low)
#define pgd_ERROR(e) \
	pr_err("%s:%d: bad pgd %#08lx\n", __FILE__, __LINE__, pgd_val(e))

/*
 * Certain architectures need to do special things when PTEs within a page
 * table are directly modified. Thus, the following hook is made available.
 */
static inline void native_set_pte(pte_t *ptep, pte_t pte)
{
	*ptep = pte;
}

static inline void native_set_pmd(pmd_t *pmdp, pmd_t pmd)
{
	*pmdp = pmd;
}

static inline void native_set_pte_atomic(pte_t *ptep, pte_t pte)
{
	native_set_pte(ptep, pte);
}

static inline void native_pmd_clear(pmd_t *pmdp)
{
	native_set_pmd(pmdp, __pmd(0));
}

static inline void native_pte_clear(struct mm_struct *mm,
				    unsigned long addr, pte_t *xp)
{
	*xp = native_make_pte(0);
}

#ifdef CONFIG_SMP
static inline pte_t native_ptep_get_and_clear(pte_t *xp)
{
	return __pte(xchg(&xp->pte_low, 0));
}
static inline pmd_t native_pmdp_get_and_clear(pmd_t *xp)
{
	return __pmd(xchg((pmdval_t *)xp, 0));
}
#else
#define native_ptep_get_and_clear(xp) native_local_ptep_get_and_clear(xp)
#define native_pmdp_get_and_clear(xp) native_local_pmdp_get_and_clear(xp)
#endif /* CONFIG_SMP */

#endif /* __ASSEMBLY__ */

#endif /* _ASM_X86_PGTABLE_2LEVEL_H_ */
