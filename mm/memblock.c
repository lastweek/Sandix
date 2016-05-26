/*
 *	Logical Memory Blocks
 *
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

#include <sandix/bug.h>
#include <sandix/numa.h>
#include <sandix/kernel.h>
#include <sandix/memblock.h>

#define MEMBLOCK_DEBUG

#ifdef MEMBLOCK_DEBUG
#define memblock_dbg(fmt...)	pr_info(fmt)
#else
#define memblock_dbg(fmt...)	do { } while (0)
#endif

static struct memblock_region memblock_memory_init_regions[INIT_MEMBLOCK_REGIONS];
static struct memblock_region memblock_reserved_init_regions[INIT_MEMBLOCK_REGIONS];

struct memblock memblock = {
	.is_bottom_up		= false,
	.current_limit		= MEMBLOCK_ALLOC_ANYWHERE,

	.memory.nr_regions	= 1,
	.memory.regions		= memblock_memory_init_regions,
	.memory.max		= INIT_MEMBLOCK_REGIONS,

	.reserved.nr_regions	= 1,
	.reserved.regions	= memblock_reserved_init_regions,
	.reserved.max		= INIT_MEMBLOCK_REGIONS,
};

static inline phys_addr_t memblock_cap_size(phys_addr_t base, phys_addr_t *size)
{
	return *size = min(*size, (phys_addr_t)ULLONG_MAX - base);
}

/**
 * memblock_add_range - add new memblock region
 * @type: memblock type to add new region into
 * @base: base address of the new region
 * @size: size of the new region
 * @nid: nid of the new region
 * @flags: flags of the new region
 *
 * Add new memblock region [@base,@base+@size) into @type.  The new region
 * is allowed to overlap with existing ones - overlaps don't affect already
 * existing regions.  @type is guaranteed to be minimal (all neighbouring
 * compatible regions are merged) after the addition.
 *
 * RETURNS:
 * 0 on success, -errno on failure.
 */
int memblock_add_range(struct memblock_type *type, phys_addr_t base,
		       phys_addr_t size, int nid, unsigned long flags)
{
	int idx, nr_new;
	struct memblock_region *region;
	phys_addr_t obase = base;
	phys_addr_t end = base + memblock_cap_size(base, &size);

	if (!size)
		return 0;

	/* special case for empty array */
	if (type->regions[0].size == 0) {
		WARN_ON(type->nr_regions != 1 || type->total_size);
		type->regions[0].base = base;
		type->regions[0].size = size;
		type->regions[0].flags = flags;
		memblock_set_region_node(&type->regions[0], nid);
		type->total_size = size;
		return 0;
	}

	return 0;
}

int memblock_reserve(phys_addr_t base, phys_addr_t size)
{
	memblock_dbg("memblock_reserve: [%#016llx-%#016llx] flags %#02lx %pF\n",
		(unsigned long long)base,
		(unsigned long long)base + size - 1,
		0UL, (void *)_RET_IP_);

	return memblock_add_range(&memblock.reserved, base, size, MAX_NR_NODES, 0);
}
