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

#ifndef _ASM_X86_PGTABLE_3LEVEL_H_
#define _ASM_X86_PGTABLE_3LEVEL_H_

/*
 * Physical Address Extension (PAE) Mode
 * Three level paging structure
 */

#ifndef __ASSEMBLY__

#define pte_ERROR(e)							\
	pr_err("%s:%d: bad pte %p(%08lx%08lx)\n",			\
	       __FILE__, __LINE__, &(e), (e).pte_high, (e).pte_low)
#define pmd_ERROR(e)							\
	pr_err("%s:%d: bad pmd %p(%016Lx)\n",				\
	       __FILE__, __LINE__, &(e), pmd_val(e))
#define pgd_ERROR(e)							\
	pr_err("%s:%d: bad pgd %p(%016Lx)\n",				\
	       __FILE__, __LINE__, &(e), pgd_val(e))

static inline void native_pte_set(pte_t *ptep, pte_t pte)
{
	BUG();
}

static inline void native_pte_set_atomic(pte_t *ptep, pte_t pte)
{
	BUG();
}

static inline void native_pmd_set(pmd_t *pmdp, pmd_t pmd)
{
	BUG();
}

static inline void native_pud_set(pud_t *pudp, pud_t pud)
{
	BUG();
}

#endif /* __ASSEMBLY__ */
#endif /* _ASM_X86_PGTABLE_3LEVEL_H_ */
