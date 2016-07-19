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

#ifndef _SANDIX_MM_ZONE_H_
#define _SANDIX_MM_ZONE_H_

#include <sandix/list.h>
#include <sandix/kernel.h>

#define MAX_ORDER	11

/*
 * Note from Understanding the Linux Kernel:
 *
 * The ZONE_DMA and ZONE_NORMAL zones include the “normal” page frames that
 * can be directly accessed by the kernel through the linear mapping in the
 * fourth gigabyte of the linear address space. Conversely, the ZONE_HIGHMEM
 * zone includes page frames that cannot be directly accessed by the kernel
 * through the linear mapping in the fourth gigabyte of linear address space.
 * The ZONE_HIGHMEM zone is always empty on 64-bit architectures.
 */
enum ZONE_TYPE {
#ifdef CONFIG_ZONE_DMA
	/*
	 * ZONE_DMA is used when there are devices that are not able
	 * to do DMA to all of addressable memory (ZONE_NORMAL). Then we
	 * carve out the portion of memory that is needed for these devices.
	 * The range is arch specific.
	 *
	 * Some examples
	 *
	 * Architecture		Limit
	 * ---------------------------
	 * parisc, ia64, sparc	<4G
	 * s390			<2G
	 * arm			Various
	 * alpha		Unlimited or 0-16MB.
	 *
	 * i386, x86_64 and multiple other arches
	 * 			<16M.
	 */
	ZONE_DMA,
#endif
#ifdef CONFIG_ZONE_DMA32
	/*
	 * x86_64 needs two ZONE_DMAs because it supports devices that are
	 * only able to do DMA to the lower 16M but also 32 bit devices that
	 * can only do DMA areas below 4G.
	 */
	ZONE_DMA32,
#endif
	/*
	 * Normal addressable memory is in ZONE_NORMAL. DMA operations can be
	 * performed on pages in ZONE_NORMAL if the DMA devices support
	 * transfers to all addressable memory.
	 */
	ZONE_NORMAL,
#ifdef CONFIG_HIGHMEM
	/*
	 * A memory area that is only addressable by the kernel through
	 * mapping portions into its own address space. This is for example
	 * used by i386 to allow the kernel to address the memory beyond
	 * 900MB. The kernel will set up special mappings (page
	 * table entries on i386) for each page that the kernel needs to
	 * access.
	 */
	ZONE_HIGHMEM,
#endif
	ZONE_MOVABLE,
#ifdef CONFIG_ZONE_DEVICE
	/*
	 * Device memory (pmem, etc.) hotplug support allows for establishing pmem,
	 * or other device driver discovered memory regions, in the memmap. This
	 * allows pfn_to_page() lookups of otherwise "device-physical" addresses
	 * which is needed for using a DAX mapping in an O_DIRECT operation, among
	 * other things.
	 */
	ZONE_DEVICE,
#endif
	__MAX_NR_ZONES
};

struct zone {

};

#ifdef CONFIG_HAVE_MEMORY_PRESENT
void memory_present(int nid, unsigned long start, unsigned long end);
#else
static inline void memory_present(int nid, unsigned long start, unsigned long end) { }
#endif /* CONFIG_HAVE_MEMORY_PRESENT */

#ifdef CONFIG_SPARSEMEM
#include <asm/sparsemem.h>

/*
 * SECTION_SHIFT    		#bits space required to store a section #
 * PA_SECTION_SHIFT		physical address to/from section number
 * PFN_SECTION_SHIFT		pfn to/from section number
 */
#define SECTIONS_SHIFT		(MAX_PHYSMEM_BITS - SECTION_SIZE_BITS)
#define PA_SECTION_SHIFT	(SECTION_SIZE_BITS)
#define PFN_SECTION_SHIFT	(SECTION_SIZE_BITS - PAGE_SHIFT)

#define NR_MEM_SECTIONS		(1UL << SECTIONS_SHIFT)
#define PAGES_PER_SECTION	(1UL << PFN_SECTION_SHIFT)
#define PAGE_SECTION_MASK	(~(PAGES_PER_SECTION - 1))

#define pfn_to_section_nr(pfn)	((pfn) >> PFN_SECTION_SHIFT)
#define section_nr_to_pfn(sec)	((sec) << PFN_SECTION_SHIFT)

#define SECTION_ALIGN_UP(pfn)	(((pfn) + PAGES_PER_SECTION - 1) & PAGE_SECTION_MASK)
#define SECTION_ALIGN_DOWN(pfn)	((pfn) & PAGE_SECTION_MASK)

struct sparsemem_section {
	/*
	 * This is, logically, a pointer to an array of struct
	 * pages. However, it is stored with some other magic.
	 * (see sparse.c::sparse_init_one_section())
	 *
	 * Additionally during early boot we encode node id of
	 * the location of the section here to guide allocation.
	 * (see sparse.c::memory_present())
	 *
	 * Making it a UL at least makes someone do a cast
	 * before using it wrong.
	 */
	unsigned long section_mem_map;

	/* See declaration of similar field in struct zone */
	unsigned long *pageblock_flags;

	/*
	 * WARNING: sparsemem_section must be a power-of-2 in size for the
	 * calculation and use of SECTION_ROOT_MASK to make sense.
	 */
};

#ifdef CONFIG_SPARSEMEM_EXTREME
#define SECTIONS_PER_ROOT       (PAGE_SIZE / sizeof(struct spasemem_section))
#else
#define SECTIONS_PER_ROOT	1
#endif

#define SECTION_NR_TO_ROOT(sec)	((sec) / SECTIONS_PER_ROOT)
#define NR_SECTION_ROOTS	DIV_ROUND_UP(NR_MEM_SECTIONS, SECTIONS_PER_ROOT)
#define SECTION_ROOT_MASK	(SECTIONS_PER_ROOT - 1)

#ifdef CONFIG_SPARSEMEM_EXTREME
extern struct sparsemem_section *sparsemem_section[NR_SECTION_ROOTS];
#else
extern struct sparsemem_section sparsemem_section[NR_SECTION_ROOTS][SECTIONS_PER_ROOT];
#endif

static inline struct sparsemem_section *__nr_to_section(unsigned long nr)
{
	if (!sparsemem_section[SECTION_NR_TO_ROOT(nr)])
		return NULL;
	return &sparsemem_section[SECTION_NR_TO_ROOT(nr)][nr & SECTION_ROOT_MASK];
}

void __init sparse_init(void);
#else
static inline void sparse_init(void) { }
#endif /* CONFIG_SPARSEMEM */

#endif /* _SANDIX_MM_ZONE_H_ */
