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

/*
 * WARNING: Not implemented!
 */

#ifndef _ASM_X86_PGTABLE_64_H_
#define _ASM_X86_PGTABLE_64_H_

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

static inline void native_pte_set(pte_t *ptep, pte_t pte)
{
	*ptep = pte;
}

static inline void native_pte_set_atomic(pte_t *ptep, pte_t pte)
{
	native_pte_set(ptep, pte);
}

static inline void native_pte_clear(pte_t *ptep)
{
	native_pte_set(ptep, __pte(0));
}

static inline void native_pmd_set(pmd_t *pmdp, pmd_t pmd)
{
	*pmdp = pmd;
}

static inline void native_pmd_clear(pmd_t *pmdp)
{
	native_pmd_set(pmdp, __pmd(0));
}

static inline void native_pud_set(pud_t *pudp, pud_t pud)
{
	*pudp = pud;
}

static inline void native_pud_clear(pud_t *pudp)
{
	native_pud_set(pud, __pud(0));
}

static inline void native_pgd_set(pgd_t *pgdp, pgd_t pgd)
{
	*pgdp = pgd;
}

static inline void native_pgd_clear(pgd_t *pgdp)
{
	native_pgd_set(pgdp, __pgd(0));
}

#endif /* __ASSEMBLY__ */
#endif /* _ASM_X86_PGTABLE_64_H_ */
