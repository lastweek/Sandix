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

static inline void native_pte_set(pte_t *ptep, pte_t pte)
{
	*ptep = pte;
}

static inline void native_pmd_set(pmd_t *pmdp, pmd_t pmd)
{
	*pmdp = pmd;
}

static inline void native_pte_set_atomic(pte_t *ptep, pte_t pte)
{
	native_pte_set(ptep, pte);
}

static inline void native_pte_clear(pte_t *ptep)
{
	native_pte_set(ptep, __pte(0));
}

static inline void native_pmd_clear(pmd_t *pmdp)
{
	native_pmd_set(pmdp, __pmd(0));
}

#endif /* __ASSEMBLY__ */
#endif /* _ASM_X86_PGTABLE_2LEVEL_H_ */
