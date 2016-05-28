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
#include <sandix/errno.h>
#include <sandix/string.h>
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
 * memblock_merge_regions - merge neighboring compatible regions
 * @type: memblock type to scan
 *
 * Scan @type and merge neighboring compatible regions.
 */
static void memblock_merge_regions(struct memblock_type *type)
{
	int i = 0;

	/* nr_regions never goes below 1 */
	while (i < type->nr_regions - 1) {
		struct memblock_region *this = &type->regions[i];
		struct memblock_region *next = &type->regions[i + 1];

		if (this->base + this->size != next->base ||
		    memblock_get_region_node(this) !=
		    memblock_get_region_node(next) ||
		    this->flags != next->flags) {
			BUG_ON(this->base + this->size > next->base);
			i++;
			continue;
		}

		this->size += next->size;

		/* move forward from next + 1, index of which is i + 2 */
		memmove(next, next + 1, (type->nr_regions - (i + 2)) * sizeof(*next));
		type->nr_regions--;
	}
}

/**
 * memblock_insert_region - insert new memblock region
 * @type:	memblock type to insert into
 * @idx:	index for the insertion point
 * @base:	base address of the new region
 * @size:	size of the new region
 * @nid:	node id of the new region
 * @flags:	flags of the new region
 *
 * Insert new memblock region [@base,@base+@size) into @type at @idx.
 * @type must already have extra room to accomodate the new region.
 */
static void memblock_insert_region(struct memblock_type *type, int idx,
				   phys_addr_t base, phys_addr_t size,
				   int nid, unsigned long flags)
{
	struct memblock_region *r = &type->regions[idx];

	BUG_ON(type->nr_regions >= type->max);
	memmove(r + 1, r, (type->nr_regions - idx) * sizeof(*r));
	r->base = base;
	r->size = size;
	r->flags = flags;
	memblock_set_region_node(r, nid);
	type->nr_regions++;
	type->total_size += size;
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
	bool insert = false;
	int idx, nr_new;
	struct memblock_region *r;
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

repeat:
	/*
	 * The following is executed twice.  Once with %false @insert and
	 * then with %true.  The first counts the number of regions needed
	 * to accomodate the new area.  The second actually inserts them.
	 */
	base = obase;
	nr_new = 0;

	for_each_memblock_type(type, r) {
		phys_addr_t rbase = r->base;
		phys_addr_t rend = rbase + r->size;

		if (rbase >= end)
			break;
		if (rend <= base)
			continue;
		/*
		 * @rgn overlaps.  If it separates the lower part of new
		 * area, insert that portion.
		 */
		if (rbase > base) {
#ifdef CONFIG_HAVE_MEMBLOCK_NODE_MAP
			WARN_ON(nid != memblock_get_region_node(r));
#endif
			WARN_ON(flags != r->flags);
			nr_new++;
			if (insert)
				memblock_insert_region(type, idx++, base,
						       rbase - base, nid,
						       flags);
		}
		/* area below @rend is dealt with, forget about it */
		base = min(rend, end);
	}

	/* insert the remaining portion */
	if (base < end) {
		nr_new++;
		if (insert)
			memblock_insert_region(type, idx, base, end - base,
					       nid, flags);
	}

	/*
	 * If this was the first round, resize array and repeat for actual
	 * insertions; otherwise, merge and return.
	 */
	if (!insert) {
		while (type->nr_regions + nr_new > type->max) {
			WARN_ONCE(1, "Expand array is NOT implemented");
			return -ENOMEM;
		}
		insert = true;
		goto repeat;
	} else {
		memblock_merge_regions(type);
		return 0;
	}
}

int memblock_add_node(phys_addr_t base, phys_addr_t size, int nid)
{
	return memblock_add_range(&memblock.memory, base, size, nid, 0);
}

int memblock_add(phys_addr_t base, phys_addr_t size)
{
	memblock_dbg("memblock_add: [%#016llx-%#016llx] flags %#02lx %pF\n",
		     (unsigned long long)base,
		     (unsigned long long)base + size - 1,
		     0UL, (void *)_RET_IP_);

	return memblock_add_range(&memblock.memory, base, size, MAX_NR_NODES, 0);
}

int memblock_reserve(phys_addr_t base, phys_addr_t size)
{
	memblock_dbg("memblock_reserve: [%#016llx-%#016llx] flags %#02lx %pF\n",
		(unsigned long long)base,
		(unsigned long long)base + size - 1,
		0UL, (void *)_RET_IP_);

	return memblock_add_range(&memblock.reserved, base, size, MAX_NR_NODES, 0);
}

static void memblock_remove_region(struct memblock_type *type, unsigned long r)
{
	type->total_size -= type->regions[r].size;
	memmove(&type->regions[r], &type->regions[r + 1],
		(type->nr_regions - (r + 1)) * sizeof(type->regions[r]));
	type->nr_regions--;

	/* Special case for empty arrays */
	if (type->nr_regions == 0) {
		WARN_ON(type->total_size != 0);
		type->nr_regions = 1;
		type->regions[0].base = 0;
		type->regions[0].size = 0;
		type->regions[0].flags = 0;
		memblock_set_region_node(&type->regions[0], MAX_NR_NODES);
	}
}

/**
 * memblock_isolate_range - isolate given range into disjoint memblocks
 * @type: memblock type to isolate range for
 * @base: base of range to isolate
 * @size: size of range to isolate
 * @start_rgn: out parameter for the start of isolated region
 * @end_rgn: out parameter for the end of isolated region
 *
 * Walk @type and ensure that regions don't cross the boundaries defined by
 * [@base,@base+@size).  Crossing regions are split at the boundaries,
 * which may create at most two more regions.  The index of the first
 * region inside the range is returned in *@start_rgn and end in *@end_rgn.
 *
 * RETURNS:
 * 0 on success, -errno on failure.
 */
static int memblock_isolate_range(struct memblock_type *type,
				  phys_addr_t base, phys_addr_t size,
				  int *start_rgn, int *end_rgn)
{
	int idx;
	phys_addr_t end = base + memblock_cap_size(base, &size);
	struct memblock_region *rgn;

	*start_rgn = *end_rgn = 0;

	if (!size)
		return 0;

	/* we'll create at most two more regions */
	while (type->nr_regions + 2 > type->max) {
		WARN_ONCE(1, "Expand array is NOT implemented");
		return -ENOMEM;
	}

	for_each_memblock_type(type, rgn) {
		phys_addr_t rbase = rgn->base;
		phys_addr_t rend = rbase + rgn->size;

		if (rbase >= end)
			break;
		if (rend <= base)
			continue;

		if (rbase < base) {
			/*
			 * @rgn intersects from below.  Split and continue
			 * to process the next region - the new top half.
			 */
			rgn->base = base;
			rgn->size -= base - rbase;
			type->total_size -= base - rbase;
			memblock_insert_region(type, idx, rbase, base - rbase,
					       memblock_get_region_node(rgn),
					       rgn->flags);
		} else if (rend > end) {
			/*
			 * @rgn intersects from above.  Split and redo the
			 * current region - the new bottom half.
			 */
			rgn->base = end;
			rgn->size -= end - rbase;
			type->total_size -= end - rbase;
			memblock_insert_region(type, idx--, rbase, end - rbase,
					       memblock_get_region_node(rgn),
					       rgn->flags);
		} else {
			/* @rgn is fully contained, record it */
			if (!*end_rgn)
				*start_rgn = idx;
			*end_rgn = idx + 1;
		}
	}

	return 0;
}

static int memblock_remove_range(struct memblock_type *type,
				 phys_addr_t base, phys_addr_t size)
{
	int start_rgn, end_rgn;
	int i, ret;

	ret = memblock_isolate_range(type, base, size, &start_rgn, &end_rgn);
	if (ret)
		return ret;

	for (i = end_rgn - 1; i >= start_rgn; i--)
		memblock_remove_region(type, i);
	return 0;
}

int memblock_remove(phys_addr_t base, phys_addr_t size)
{
	return memblock_remove_range(&memblock.memory, base, size);
}

static void memblock_dump(struct memblock_type *type, char *name)
{
	unsigned long long base, size;
	unsigned long flags;
	int idx;
	struct memblock_region *rgn;

	pr_info(" %s.nr_regions  = 0x%lx\n", name, type->nr_regions);

	for_each_memblock_type(type, rgn) {
		char nid_buf[32] = "";

		base = rgn->base;
		size = rgn->size;
		flags = rgn->flags;
#ifdef CONFIG_HAVE_MEMBLOCK_NODE_MAP
		if (memblock_get_region_node(rgn) != MAX_NR_NODES)
			snprintf(nid_buf, sizeof(nid_buf), " on node %d",
				 memblock_get_region_node(rgn));
#endif
		pr_info(" %s[%#x]\t[%#016llx-%#016llx], %#llx bytes%s flags: %#lx\n",
			name, idx, base, base + size - 1, size, nid_buf, flags);
	}
}

void memblock_dump_all(void)
{
	pr_info("MEMBLOCK configuration:\n");
	pr_info(" memory size = %#llx reserved size = %#llx\n",
		(unsigned long long)memblock.memory.total_size,
		(unsigned long long)memblock.reserved.total_size);

	memblock_dump(&memblock.memory, "memory");
	memblock_dump(&memblock.reserved, "reserved");
}
