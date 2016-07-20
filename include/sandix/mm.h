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

#ifndef _SANDIX_MM_H_
#define _SANDIX_MM_H_

#include <asm/page.h>
#include <asm/pgtable.h>

#include <sandix/types.h>
#include <sandix/kernel.h>
#include <sandix/atomic.h>
#include <sandix/mm_zone.h>
#include <sandix/spinlock.h>

#define offset_in_page(p)	((unsigned long)(p) & ~PAGE_MASK)

/* to align the pointer to the (next) page boundary */
#define PAGE_ALIGN(addr)	ALIGN(addr, PAGE_SIZE)

/* test whether an address (unsigned long or pointer) is aligned to PAGE_SIZE */
#define PAGE_ALIGNED(addr)	IS_ALIGNED((unsigned long)addr, PAGE_SIZE)

struct mm_struct;

/*
 * This struct defines a memory VMM memory area. There is one of these
 * per VM-area/task.  A VM area is any part of the process virtual memory
 * space that has a special rule for the page-fault handlers (ie a shared
 * library, the executable area etc).
 */
struct vm_area_struct {
	unsigned long vm_start, vm_end;
	struct vm_area_struct *vm_next, *vm_prev;
	struct mm_struct *vm_mm;
};

/**
 * struct mm_struct
 * @mmap:		list of VMAs
 * @mmap_base:		base of mmap area
 * @pgd:		global page table
 * @mm_users:		how many users with userspace
 * @mm_count:		how many references to this struct
 * @page_table_lock:	protects page table and some counters
 */
struct mm_struct {
	struct vm_area_struct	*mmap;
	pgd_t 			*pgd;
	atomic_t		mm_users;
	atomic_t		mm_count;
	spinlock_t		page_table_lock;
	unsigned long		mmap_base;
	unsigned long		start_code, end_code;
	unsigned long		start_data, end_data;
	unsigned long		start_brk, brk;
	unsigned long		start_stack;
};

extern unsigned long max_low_pfn;
extern unsigned long min_low_pfn;
extern unsigned long max_pfn;

/*
 * A number of key systems in x86 including ioremap() rely on the assumption
 * that high_memory defines the upper bound on direct map memory, then end
 * of ZONE_NORMAL.  Under CONFIG_DISCONTIG this means that max_low_pfn and
 * highstart_pfn must be the same; there must be no gap between ZONE_NORMAL
 * and ZONE_HIGHMEM.
 */
extern void *high_memory;

void __init mem_init(void);
void __init sparse_memory_present_with_active_regions(int nid);

#ifdef CONFIG_HAVE_MEMBLOCK_NODE_MAP
/*
 * With CONFIG_HAVE_MEMBLOCK_NODE_MAP set, an architecture may initialise its
 * zones, allocate the backing mem_map and account for memory holes in a more
 * architecture independent manner. This is a substitute for creating the
 * zone_sizes[] and zholes_size[] arrays and passing them to
 * free_area_init_node()
 *
 * An architecture is expected to register range of page frames backed by
 * physical memory with memblock_add[_node]() before calling
 * free_area_init_nodes() passing in the PFN each zone ends at. At a basic
 * usage, an architecture is expected to do something like
 *
 * unsigned long max_zone_pfns[MAX_NR_ZONES] = {max_dma, max_normal_pfn,
 * 							 max_highmem_pfn};
 * for_each_valid_physical_page_range()
 * 	memblock_add_node(base, size, nid)
 * free_area_init_nodes(max_zone_pfns);
 *
 * free_bootmem_with_active_regions() calls free_bootmem_node() for each
 * registered physical page range.  Similarly
 * sparse_memory_present_with_active_regions() calls memory_present() for
 * each range when SPARSEMEM is enabled.
 *
 * See mm/page_alloc.c for more information on each function exposed by
 * CONFIG_HAVE_MEMBLOCK_NODE_MAP.
 */
void __init free_area_init_nodes(unsigned long *zone_max_pfns);
#endif /* CONFIG_HAVE_MEMBLOCK_NODE_MAP */

#endif /* _SANDIX_MM_H_ */
