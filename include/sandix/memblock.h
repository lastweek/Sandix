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

#include <sandix/types.h>

#define INIT_MEMBLOCK_REGIONS	128

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

int memblock_add_node(phys_addr_t base, phys_addr_t size, int nid);
int memblock_add(phys_addr_t base, phys_addr_t size);
int memblock_reserve(phys_addr_t base, phys_addr_t size);
int memblock_remove(phys_addr_t base, phys_addr_t size);
void memblock_dump_all(void);

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

#define for_each_memblock_type(memblock_type, rgn)			\
	idx = 0;							\
	rgn = &memblock_type->regions[idx];				\
	for (idx = 0; idx < memblock_type->nr_regions;			\
	     idx++,rgn = &memblock_type->regions[idx])

#endif /* _SANDIX_MEMBLOCK_H_ */
