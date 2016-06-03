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

#ifndef _ASM_GENERIC_FIXMAP_H_
#define _ASM_GENERIC_FIXMAP_H_

#include <asm/pgtable.h>

/*
 * Compile-time virtual memory allocation
 */

#define __fix_to_virt(x)	(FIXADDR_TOP - ((x) << PAGE_SHIFT))
#define __virt_to_fix(x)	((FIXADDR_TOP - ((x) & PAGE_MASK)) >> PAGE_SHIFT)

static inline unsigned long fix_to_virt(const unsigned int idx)
{

	BUILD_BUG_ON(idx >= __end_of_fixed_addresses);
	return __fix_to_virt(idx);
}

static inline unsigned long virt_to_fix(const unsigned long vaddr)
{
	BUG_ON(vaddr >= FIXADDR_TOP || vaddr < FIXADDR_START);
	return __virt_to_fix(vaddr);
}

/*
 * Provide some reasonable defaults for page flags.
 * Not all architectures use all of these different types and some
 * architectures use different names.
 */
#ifndef FIXMAP_PAGE_NORMAL
#define FIXMAP_PAGE_NORMAL PAGE_KERNEL
#endif

#ifndef FIXMAP_PAGE_RO
#define FIXMAP_PAGE_RO PAGE_KERNEL_RO
#endif

#ifndef FIXMAP_PAGE_NOCACHE
#define FIXMAP_PAGE_NOCACHE PAGE_KERNEL_NOCACHE
#endif

#ifndef FIXMAP_PAGE_IO
#define FIXMAP_PAGE_IO PAGE_KERNEL_IO
#endif

#ifndef FIXMAP_PAGE_CLEAR
#define FIXMAP_PAGE_CLEAR __pgprot(0)
#endif

#define set_fixmap(idx, phys)	__set_fixmap(idx, phys, FIXMAP_PAGE_NORMAL)
#define clear_fixmap(idx)	__set_fixmap(idx, 0, FIXMAP_PAGE_CLEAR)

/* Return a pointer with offset calculated */
#define __set_fixmap_offset(idx, phys, flags)		      \
({							      \
	unsigned long addr;				      \
	__set_fixmap(idx, phys, flags);			      \
	addr = fix_to_virt(idx) + ((phys) & (PAGE_SIZE - 1)); \
	addr;						      \
})

#define set_fixmap_offset(idx, phys)				\
	__set_fixmap_offset(idx, phys, FIXMAP_PAGE_NORMAL)

#endif /* _ASM_GENERIC_FIXMAP_H_ */
