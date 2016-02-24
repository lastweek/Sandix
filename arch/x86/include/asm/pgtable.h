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

#include <asm/page.h>
#include <sandix/const.h>
#include <sandix/types.h>

#include <asm/pgtable_types.h>

#ifdef CONFIG_X86_32
# include <asm/pgtable_32.h>
#else
# include <asm/pgtable_64.h>
#endif

#define set_pte(ptep, pte)		native_set_pte(ptep, pte)
#define set_pte_atomic(ptep, pte)	native_set_pte_atomic(ptep, pte)

#define set_pmd(pmdp, pmd)		native_set_pmd(pmdp, pmd)

#define clear_pte(addr, ptep)		native_clear_pte(addr, ptep)
#define clear_pmd(pmd)			native_clear_pmd(pmd)








































#endif /* _ASM_X86_PGTABLE_H_ */
