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

#ifndef _ASM_GENERIC_PGTABLE_H_
#define _ASM_GENERIC_PGTABLE_H_

#if 4 - defined(__PGTABLE_PUD_FOLDED) - defined(__PGTABLE_PMD_FOLDED) != \
	CONFIG_PGTABLE_LEVELS
#error "CONFIG_PGTABLE_LEVELS is not consistent with __PGTABLE_{PUD,PMD}_FOLDED"
#endif

/*
 * When walking page tables, get the address of the next boundary,
 * or the end address of the range if that comes earlier.
 *
 * Although no vma end wraps to 0, rounded up __boundary may wrap
 * to 0 throughout, so do the minus in case bad things happen.
 */

#define pgd_addr_end(addr, end)						\
({									\
	unsigned long __boundary = ((addr) + PGDIR_SIZE) & PGDIR_MASK;	\
	(__boundary -1 > (end) - 1) ? __boundary : (end);		\
})

#ifndef pud_addr_end
#define pud_addr_end(addr, end)						\
({									\
	unsigned long __boundary = ((addr) + PUD_SIZE) & PUD_MASK;	\
	(__boundary -1 > (end) - 1) ? __boundary : (end);		\
})
#endif

#ifndef pmd_addr_end
#define pmd_addr_end(addr, end)						\
({									\
	unsigned long __boundary = ((addr) + PMD_SIZE) & PMD_MASK;	\
	(__boundary -1 > (end) - 1) ? __boundary : (end);		\
})
#endif

#endif /* _ASM_GENERIC_PGTABLE_H_ */
