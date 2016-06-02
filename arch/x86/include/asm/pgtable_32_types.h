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

#ifndef _ASM_X86_PGTABLE_32_TYPES_H_
#define _ASM_X86_PGTABLE_32_TYPES_H_

#ifdef CONFIG_X86_PAE
# include <asm/pgtable-3level-types.h>
# define PMD_SIZE	(1UL << PMD_SHIFT)
# define PMD_MASK	(~(PMD_SIZE - 1))
#else
# include <asm/pgtable-2level-types.h>
#endif

#define PGDIR_SIZE	(1UL << PGDIR_SHIFT)
#define PGDIR_MASK	(~(PGDIR_SIZE - 1))

/*
 * Just any arbitrary offset to the start of the vmalloc VM area: the
 * current 8MB value just means that there will be a 8MB "hole" after the
 * physical memory until the kernel virtual memory starts.  That means that
 * any out-of-bounds memory accesses will hopefully be caught.
 * The vmalloc() routines leaves a hole of 4kB between each vmalloced
 * area for the same reason. ;)
 */
#define VMALLOC_OFFSET		(8 * 1024 * 1024)

#define VMALLOC_START		((unsigned long)high_memory + VMALLOC_OFFSET)

#ifdef CONFIG_X86_PAE
# define LAST_PKMAP		512
#else
# define LAST_PKMAP		1024
#endif

#define PKMAP_BASE		((FIXADDR_START - PAGE_SIZE * (LAST_PKMAP + 1)) & PMD_MASK)

#ifdef CONFIG_HIGHMEM
# define VMALLOC_END		(PKMAP_BASE - 2 * PAGE_SIZE)
#else
# define VMALLOC_END		(FIXADDR_START - 2 * PAGE_SIZE)
#endif

#define __VMALLOC_RESERVE	(128 << 20)
#define MAXMEM			(VMALLOC_END - PAGE_OFFSET - __VMALLOC_RESERVE)
#define MAXMEM_PFN		PFN_DOWN(MAXMEM)

#endif /* _ASM_X86_PGTABLE_32_TYPES_H_ */
