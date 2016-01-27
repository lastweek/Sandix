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
 * Wed Jan 27 10:19:42 CST 2016
 * XXX
 * Things are pretty complicated in 64-bit! Let me handle 32-bit first...
 */

#ifndef _ASM_X86_PAGE_64_H_
#define _ASM_X86_PAGE_64_H_

#ifndef _ASM_X86_PAGE_H_
# error "Please do not include this file directly"
#endif

/*
 * Set __PAGE_OFFSET to the most negative possible address +
 * PGDIR_SIZE*16 (pgd slot 272).  The gap is to allow a space for a
 * hypervisor to fit.  Choosing 16 slots here is arbitrary, but it's
 * what Xen requires.
 */

#define __PAGE_OFFSET           _AC(0xffff880000000000, UL)

#define THREAD_SIZE_ORDER	2
#define THREAD_SIZE		(PAGE_SIZE << THREAD_SIZE_ORDER)

/*
 * See Documentation/x86/x86_64/mm.txt for a description
 * of the memory map.
 */

#define __PHYSICAL_MASK_SHIFT	46
#define __VIRTUAL_MASK_SHIFT	47

#define __phys_addr(x)		BUG()
#define __phys_addr_symbol(x)	BUG()

#endif /* _ASM_X86_PAGE_64_H_ */
