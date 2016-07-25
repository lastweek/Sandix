/*
 *	Logical Memory Blocks
 *
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

#ifndef _SANDIX_MEMBLOCK_H_
#define _SANDIX_MEMBLOCK_H_

#include <sandix/numa.h>
#include <sandix/types.h>

#define INIT_MEMBLOCK_REGIONS	128

/* Definition of memblock flags. */
enum {
	MEMBLOCK_NONE		= 0x0,	/* No special request */
	MEMBLOCK_HOTPLUG	= 0x1,	/* hotpluggable region */
	MEMBLOCK_MIRROR		= 0x2,	/* mirrored region */
	MEMBLOCK_NOMAP		= 0x4,	/* don't add to kernel direct mapping */
};

/* Flags for memblock_alloc_base() and __memblock_alloc_base() */
#define MEMBLOCK_ALLOC_ANYWHERE		(~(phys_addr_t)0)
#define MEMBLOCK_ALLOC_ACCESSIBLE	0

#define MEMBLOCK_NONE		0x0	/* No special request */
#define MEMBLOCK_HOTPLUG	0x1	/* hotpluggable region */
#define MEMBLOCK_MIRROR		0x2	/* mirrored region */
#define MEMBLOCK_NOMAP		0x4	/* don't add to kernel direct-mapping */

struct memblock_region {
	phys_addr_t		base;
	phys_addr_t		size;
	unsigned long		flags;
#ifdef CONFIG_HAVE_MEMBLOCK_NODE_MAP
	int			nid;
#endif
};

struct memblock_type {
	unsigned long		nr_regions;
	unsigned long		max;
	phys_addr_t		total_size;
	struct memblock_region	*regions;
};

struct memblock {
	bool			is_bottom_up;
	phys_addr_t		current_limit;
	struct memblock_type	memory;
	struct memblock_type	reserved;
};

extern struct memblock memblock;

unsigned long choose_memblock_flags(void);
void memblock_set_current_limit(phys_addr_t limit);
phys_addr_t memblock_get_current_limit(void);
int memblock_add_node(phys_addr_t base, phys_addr_t size, int nid);
int memblock_add(phys_addr_t base, phys_addr_t size);
int memblock_reserve(phys_addr_t base, phys_addr_t size);
int memblock_remove(phys_addr_t base, phys_addr_t size);
int memblock_free(phys_addr_t base, phys_addr_t size);
void memblock_trim_memory(phys_addr_t align);
void memblock_dump_all(void);

phys_addr_t __init memblock_find_in_range_node(phys_addr_t size,
					       phys_addr_t align,
					       phys_addr_t start, phys_addr_t end,
					       int nid, unsigned long flags);

phys_addr_t __init memblock_find_in_range(phys_addr_t start, phys_addr_t end,
					  phys_addr_t size, phys_addr_t align);

static inline bool memblock_bottom_up(void)
{
	return false;
}

static inline bool movable_node_is_enabled(void)
{
	return false;
}

static inline bool memblock_is_hotpluggable(struct memblock_region *m)
{
	return m->flags & MEMBLOCK_HOTPLUG;
}

static inline bool memblock_is_mirror(struct memblock_region *m)
{
	return m->flags & MEMBLOCK_MIRROR;
}

static inline bool memblock_is_nomap(struct memblock_region *m)
{
	return m->flags & MEMBLOCK_NOMAP;
}

#ifdef CONFIG_HAVE_MEMBLOCK_NODE_MAP
static inline void memblock_set_region_node(struct memblock_region *r, int nid)
{
	r->nid = nid;
}
static inline int memblock_get_region_node(const struct memblock_region *r)
{
	return r->nid;
}
#else
static inline void memblock_set_region_node(struct memblock_region *r, int nid) { }
static inline int memblock_get_region_node(const struct memblock_region *r) { return 0; }
#endif

#ifdef CONFIG_HAVE_MEMBLOCK_NODE_MAP
int memblock_search_pfn_nid(unsigned long pfn, unsigned long *start_pfn,
			    unsigned long  *end_pfn);

void __next_mem_pfn_range(int *idx, int nid, unsigned long *out_start_pfn,
			  unsigned long *out_end_pfn, int *out_nid);

int __init memblock_set_node(phys_addr_t base, phys_addr_t size,
			     struct memblock_type *type, int nid);

/**
 * for_each_mem_pfn_range - early memory pfn range iterator
 * @i: an integer used as loop variable
 * @nid: node selector, %MAX_NR_NODES for all nodes
 * @p_start: ptr to ulong for start pfn of the range, can be %NULL
 * @p_end: ptr to ulong for end pfn of the range, can be %NULL
 * @p_nid: ptr to int for nid of the range, can be %NULL
 *
 * Walks over configured memory ranges.
 */
#define for_each_mem_pfn_range(i, nid, p_start, p_end, p_nid)		\
	for (i = -1, __next_mem_pfn_range(&i, nid, p_start, p_end, p_nid); \
	     i >= 0; __next_mem_pfn_range(&i, nid, p_start, p_end, p_nid))
#endif /* CONFIG_HAVE_MEMBLOCK_NODE_MAP */

#define for_each_memblock(memblock_type, r)						\
	for (r = memblock.memblock_type.regions;					\
	     r < (memblock.memblock_type.regions + memblock.memblock_type.nr_regions);	\
	     r++)

#define for_each_memblock_type(memblock_type, rgn)			\
	idx = 0;							\
	rgn = &memblock_type->regions[idx];				\
	for (idx = 0; idx < memblock_type->nr_regions;			\
	     idx++,rgn = &memblock_type->regions[idx])

/**
 * for_each_mem_range - iterate through memblock areas from type_a and not
 * included in type_b. Or just type_a if type_b is NULL.
 * @i: u64 used as loop variable
 * @type_a: ptr to memblock_type to iterate
 * @type_b: ptr to memblock_type which excludes from the iteration
 * @nid: node selector, %NUMA_NO_NODE for all nodes
 * @flags: pick from blocks based on memory attributes
 * @p_start: ptr to phys_addr_t for start address of the range, can be %NULL
 * @p_end: ptr to phys_addr_t for end address of the range, can be %NULL
 * @p_nid: ptr to int for nid of the range, can be %NULL
 */
#define for_each_mem_range(i, type_a, type_b, nid, flags,		\
			   p_start, p_end, p_nid)			\
	for (i = 0, __next_mem_range(&i, nid, flags, type_a, type_b,	\
				     p_start, p_end, p_nid);		\
	     i != (u64)ULLONG_MAX;					\
	     __next_mem_range(&i, nid, flags, type_a, type_b,		\
			      p_start, p_end, p_nid))

/**
 * for_each_mem_range_rev - reverse iterate through memblock areas from
 * type_a and not included in type_b. Or just type_a if type_b is NULL.
 * @i: u64 used as loop variable
 * @type_a: ptr to memblock_type to iterate
 * @type_b: ptr to memblock_type which excludes from the iteration
 * @nid: node selector, %NUMA_NO_NODE for all nodes
 * @flags: pick from blocks based on memory attributes
 * @p_start: ptr to phys_addr_t for start address of the range, can be %NULL
 * @p_end: ptr to phys_addr_t for end address of the range, can be %NULL
 * @p_nid: ptr to int for nid of the range, can be %NULL
 */
#define for_each_mem_range_rev(i, type_a, type_b, nid, flags,		\
			       p_start, p_end, p_nid)			\
	for (i = (u64)ULLONG_MAX,					\
		     __next_mem_range_rev(&i, nid, flags, type_a, type_b,\
					 p_start, p_end, p_nid);	\
	     i != (u64)ULLONG_MAX;					\
	     __next_mem_range_rev(&i, nid, flags, type_a, type_b,	\
				  p_start, p_end, p_nid))

/**
 * for_each_free_mem_range - iterate through free memblock areas
 * @i: u64 used as loop variable
 * @nid: node selector, %NUMA_NO_NODE for all nodes
 * @flags: pick from blocks based on memory attributes
 * @p_start: ptr to phys_addr_t for start address of the range, can be %NULL
 * @p_end: ptr to phys_addr_t for end address of the range, can be %NULL
 * @p_nid: ptr to int for nid of the range, can be %NULL
 *
 * Walks over free (memory && !reserved) areas of memblock.  Available as
 * soon as memblock is initialized.
 */
#define for_each_free_mem_range(i, nid, flags, p_start, p_end, p_nid)	\
	for_each_mem_range(i, &memblock.memory, &memblock.reserved,	\
			   nid, flags, p_start, p_end, p_nid)

/**
 * for_each_free_mem_range_reverse - rev-iterate through free memblock areas
 * @i: u64 used as loop variable
 * @nid: node selector, %NUMA_NO_NODE for all nodes
 * @flags: pick from blocks based on memory attributes
 * @p_start: ptr to phys_addr_t for start address of the range, can be %NULL
 * @p_end: ptr to phys_addr_t for end address of the range, can be %NULL
 * @p_nid: ptr to int for nid of the range, can be %NULL
 *
 * Walks over free (memory && !reserved) areas of memblock in reverse
 * order.  Available as soon as memblock is initialized.
 */
#define for_each_free_mem_range_reverse(i, nid, flags, p_start, p_end,	\
					p_nid)				\
	for_each_mem_range_rev(i, &memblock.memory, &memblock.reserved,	\
			       nid, flags, p_start, p_end, p_nid)

/* Use memblock to allocate bootmem */
#ifndef CONFIG_BITMAP_BOOTMEM

void * __init memblock_virt_alloc_try_nid_nopanic(
				phys_addr_t size, phys_addr_t align,
				phys_addr_t min_addr, phys_addr_t max_addr,
				int nid);

static inline void *memblock_virt_alloc_nopanic(phys_addr_t size, phys_addr_t align)
{
	return memblock_virt_alloc_try_nid_nopanic(size, align, 0, 0, NUMA_NO_NODE);
}

static inline void *memblock_virt_alloc_node_nopanic(phys_addr_t size, int nid)
{
	return memblock_virt_alloc_try_nid_nopanic(size, 0, 0, 0, nid);
}

#endif /* CONFIG_BITMAP_BOOTMEM */

#endif /* _SANDIX_MEMBLOCK_H_ */
