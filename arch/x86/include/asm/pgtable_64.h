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

#ifndef _ASM_X86_PGTABLE_64_H_
#define _ASM_X86_PGTABLE_64_H_

#include <asm/pgtable_64_types.h>

#ifndef __ASSEMBLY__

#define pte_ERROR(e)					\
	pr_err("%s:%d: bad pte %p(%016lx)\n",		\
	       __FILE__, __LINE__, &(e), pte_val(e))
#define pmd_ERROR(e)					\
	pr_err("%s:%d: bad pmd %p(%016lx)\n",		\
	       __FILE__, __LINE__, &(e), pmd_val(e))
#define pud_ERROR(e)					\
	pr_err("%s:%d: bad pud %p(%016lx)\n",		\
	       __FILE__, __LINE__, &(e), pud_val(e))
#define pgd_ERROR(e)					\
	pr_err("%s:%d: bad pgd %p(%016lx)\n",		\
	       __FILE__, __LINE__, &(e), pgd_val(e))

static inline void native_pte_clear(struct mm_struct *mm, unsigned long addr,
				    pte_t *ptep)
{
	*ptep = native_make_pte(0);
}

static inline void native_set_pte(pte_t *ptep, pte_t pte)
{
	*ptep = pte;
}

static inline void native_set_pte_atomic(pte_t *ptep, pte_t pte)
{
	native_set_pte(ptep, pte);
}

static inline void native_set_pmd(pmd_t *pmdp, pmd_t pmd)
{
	*pmdp = pmd;
}

static inline void native_pmd_clear(pmd_t *pmdp)
{
	native_pmd_set(pmdp, native_make_pmd(0));
}

static inline pte_t native_ptep_get_and_clear(pte_t *xp)
{
#ifdef CONFIG_SMP
	return native_make_pte(xchg(&xp->pte, 0));
#else
	/* native_local_ptep_get_and_clear,
	   but duplicated because of cyclic dependency */
	pte_t ret = *xp;
	native_pte_clear(NULL, 0, xp);
	return ret;
#endif
}

static inline pmd_t native_pmdp_get_and_clear(pmd_t *xp)
{
#ifdef CONFIG_SMP
	return native_make_pmd(xchg(&xp->pmd, 0));
#else
	/* native_local_pmdp_get_and_clear,
	   but duplicated because of cyclic dependency */
	pmd_t ret = *xp;
	native_pmd_clear(xp);
	return ret;
#endif
}

static inline void native_set_pud(pud_t *pudp, pud_t pud)
{
	*pudp = pud;
}

static inline void native_pud_clear(pud_t *pudp)
{
	native_set_pud(pudp, native_make_pud(0));
}

static inline void native_set_pgd(pgd_t *pgdp, pgd_t pgd)
{
	*pgdp = pgd;
}

static inline void native_pgd_clear(pgd_t *pgdp)
{
	native_set_pgd(pgdp, native_make_pgd(0));
}

#endif /* __ASSEMBLY__ */

#endif /* _ASM_X86_PGTABLE_64_H_ */
