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
 *	You should have received a copy of the GNU General Public License along
 *	with this program; if not, write to the Free Software Foundation, Inc.,
 *	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _ASM_X86_PAGE_H_
#define _ASM_X86_PAGE_H_

#include <sandix/pfn.h>
#include <sandix/const.h>
#include <sandix/types.h>

#ifdef CONFIG_X86_32
# include <asm/page_32.h>
#else
# include <asm/page_64.h>
#endif

/*
 * PAGE_SHIFT determines the page size.
 * Sandix only support 4 KB pages.
 */

#define PAGE_SHIFT		12
#define PAGE_SIZE		(_AC(1, UL) << PAGE_SHIFT)
#define PAGE_MASK		(~(PAGE_SIZE - 1))

#define PMD_PAGE_SIZE		(_AC(1, UL) << PMD_SHIFT)
#define PMD_PAGE_MASK		(~(PMD_PAGE_SIZE - 1))

#define PUD_PAGE_SIZE		(_AC(1, UL) << PUD_SHIFT)
#define PUD_PAGE_MASK		(~(PUD_PAGE_SIZE - 1))

/* Use 1ULL instead of 1UL because of PAE... */
#define __PHYSICAL_MASK		((phys_addr_t)((1ULL << __PHYSICAL_MASK_SHIFT) - 1))
#define __VIRTUAL_MASK		((1UL << __VIRTUAL_MASK_SHIFT) - 1)

/*
 * Cast *PAGE_MASK to a signed type so that it is sign-extended if
 * virtual addresses are 32-bits but physical addresses are larger
 * (ie, 32-bit PAE). 
 */

#define PHYSICAL_PAGE_MASK	(((signed long)PAGE_MASK) & __PHYSICAL_MASK)
#define PHYSICAL_PMD_PAGE_MASK	(((signed long)PMD_PAGE_MASK) & __PHYSICAL_MASK)
#define PHYSICAL_PUD_PAGE_MASK	(((signed long)PUD_PAGE_MASK) & __PHYSICAL_MASK)

#define PAGE_OFFSET		((unsigned long)__PAGE_OFFSET)

#define __pa(x)			__phys_addr((unsigned long)x)
#define __pa_symbol(x)		__pa(x)
#define __va(x)			((void *)((unsigned long)(x) + PAGE_OFFSET))

#ifndef __ASSEMBLY__
extern unsigned long max_low_pfn_mapped;
extern unsigned long max_pfn_mapped;
#endif

#endif /* _ASM_X86_PAGE_H_ */
