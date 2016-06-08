/*
 *	Copyright (C) 2016 Yizhou Shan <shan13@purdue.edu>
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

#ifndef _ASM_GENERIC_TLBFLUSH_H_
#define _ASM_GENERIC_TLBFLUSH_H_

/*
 * This is a dummy tlbflush implementation that can be used on all no-mmu
 * architectures. If you have an MMU, you need to write your own functions.
 */
#ifdef CONFIG_MMU
#error You need to implement an architecture specific asm/tlbflush.h
#endif

#include <sandix/bug.h>

static inline void flush_tlb_mm(struct mm_struct *mm)
{
	BUG();
}

#endif /* _ASM_GENERIC_TLBFLUSH_H_ */
