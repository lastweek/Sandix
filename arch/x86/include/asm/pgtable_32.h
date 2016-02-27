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

#ifndef _ASM_X86_PGTABLE_32_H_
#define _ASM_X86_PGTABLE_32_H_

/*
 * Note that, Sandix kernel assumes 4-level paging framework. For x86_64,
 * it just fit in. However, for x86_32, which has two paging modes, we assume
 * another 3-level paging framework. These are two different framworks: the
 * 4-level abstraction is for kernel, the 3-level abstraction is for x86_32.
 *
 * When in x86_32, we have to fold PUD into PGD, so that the 3-level framwork
 * can fit with kernel's 4-level framwork.
 *
 * Back to x86_32 with PAE disabled, hardware uses 2-level paging structure, we
 * have to fold PMD into PUD, so that it can fit with x86_32's 3-level paging
 * framwork abstraction. If PAE is enabled, the hardware uses 3-level paging
 * structure, which just fit with x86_32's 3-level paging framework abstraction.
 * (Both circumstances fold PUD into PGD as described above.)
 */

/*
 * WARNING: PAE is not implemented!
 */

#ifdef CONFIG_X86_PAE
# include <asm/pgtable-3level.h>
#else
# include <asm/pgtable-2level.h>
#endif

#endif /* _ASM_X86_PGTABLE_32_H_ */
