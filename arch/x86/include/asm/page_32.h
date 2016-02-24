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

#ifndef _ASM_X86_PAGE_32_H_
#define _ASM_X86_PAGE_32_H_

/*
 * This handles the memory map.
 *
 * A __PAGE_OFFSET of 0xC0000000 means that the kernel has
 * a virtual address space of one gigabyte, which limits the
 * amount of physical memory you can use to about 950MB.
 *
 * If you want more physical memory than this then see the CONFIG_HIGHMEM4G
 * and CONFIG_HIGHMEM64G options in the kernel configuration.
 */
#define __PAGE_OFFSET		_AC(CONFIG_PAGE_OFFSET, UL)

#define THREAD_SIZE_ORDER	1
#define THREAD_SIZE		(PAGE_SIZE << THREAD_SIZE_ORDER)

/*
 * Physical/Virtual address mask.
 * When PAE enabled, 44 = 32 + 12
 */

#ifdef CONFIG_X86_PAE
# define __PHYSICAL_MASK_SHIFT	44
# define __VIRTUAL_MASK_SHIFT	32
#else
# define __PHYSICAL_MASK_SHIFT	32
# define __VIRTUAL_MASK_SHIFT	32
#endif

#define __phys_addr(x)		((x) - PAGE_OFFSET)
#define __phys_addr_symbol(x)	__phys_addr(x)

#endif /* _ASM_X86_PAGE_32_H_ */
