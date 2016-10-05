/*
 *	Copyright (C) 2016 Yizhou Shan <shan13@purdue.edu>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 */

#include <sandix/mm.h>
#include <sandix/numa.h>
#include <sandix/page.h>
#include <sandix/export.h>
#include <sandix/mm_zone.h>
#include <sandix/memblock.h>
#include <sandix/nodemask.h>

/*
 * Bitmask arrays of node states
 */
struct nodemask node_states[__NR_NODE_STATES] = {
	[NODE_POSSIBLE]		= { { [0] = 1UL } },
	[NODE_ONLINE]		= { { [0] = 1UL } },

/*
 * Only set a predefined value for non-NUMA configured
 * kernel. A NUMA configured kernel will setup following
 * data at boot time.
 */
#ifndef CONFIG_NUMA
	[NODE_NORMAL_MEMORY]	= { { [0] = 1UL } },
#ifdef CONFIG_HIGHMEM
	[NODE_HIGH_MEMORY]	= { { [0] = 1UL } },
#endif
#ifdef CONFIG_MOVABLE_NODE
	[NODE_MEMORY]		= { { [0] = 1UL } },
#endif
	[NODE_CPU]		= { { [0] = 1UL } },
#endif /* CONFIG_NUMA */
};
EXPORT_SYMBOL(node_states);

static char * const zone_names[__MAX_NR_ZONES] = {
#ifdef CONFIG_ZONE_DMA
	 "DMA",
#endif
#ifdef CONFIG_ZONE_DMA32
	 "DMA32",
#endif
	 "Normal",
#ifdef CONFIG_HIGHMEM
	 "HighMem",
#endif
	 "Movable",
#ifdef CONFIG_ZONE_DEVICE
	 "Device",
#endif
};

static unsigned long nr_kernel_pages;
static unsigned long nr_all_pages;
static unsigned long dma_reserve;

static unsigned long __initdata arch_zone_lowest_possible_pfn[__MAX_NR_ZONES];
static unsigned long __initdata arch_zone_highest_possible_pfn[__MAX_NR_ZONES];
static unsigned long __initdata zone_movable_pfn[MAX_NR_NODES];

#if MAX_NR_NODES > 1
int nr_node_ids __read_mostly = MAX_NR_NODES;
int nr_online_nodes __read_mostly = 1;
EXPORT_SYMBOL(nr_node_ids);
EXPORT_SYMBOL(nr_online_nodes);

static void __init setup_nr_node_ids(void)
{
	/* Walk through node_possible_map */
}

#else
static inline void setup_nr_node_ids(void) { }
#endif

/**
 * sparse_memory_present_with_active_regions - Call memory_present for each active range
 * @nid: The node to call memory_present for. If MAX_NR_NODES, all nodes will be used.
 *
 * If an architecture guarantees that all ranges registered contain no holes and may
 * be freed, this function may be used instead of calling memory_present() manually.
 */
void __init sparse_memory_present_with_active_regions(int nid)
{
	unsigned long start_pfn, end_pfn;
	int i, this_nid;

	for_each_mem_pfn_range(i, nid, &start_pfn, &end_pfn, &this_nid)
		/* memory_present(this_nid, start_pfn, end_pfn); */
		;
}

/* Find the lowest pfn for a node */
static unsigned long __init find_min_pfn_for_node(int nid)
{
	unsigned long min_pfn = ULONG_MAX;
	unsigned long start_pfn;
	int i;

	for_each_mem_pfn_range(i, nid, &start_pfn, NULL, NULL)
		min_pfn = min(min_pfn, start_pfn);

	if (min_pfn == ULONG_MAX) {
		pr_warn("Could not find start_pfn for node %d\n", nid);
		return 0;
	}

	return min_pfn;
}

/**
 * find_min_pfn_with_active_regions - Find the minimum PFN registered
 *
 * It returns the minimum PFN based on information provided via
 * memblock_set_node().
 */
unsigned long __init find_min_pfn_with_active_regions(void)
{
	return find_min_pfn_for_node(MAX_NR_NODES);
}

static void __init find_zone_movable_pfns_for_nodes(void)
{

}

/*
 * Return the number of pages a zone spans in a node, including holes
 * present_pages = zone_spanned_pages_in_node() - zone_absent_pages_in_node()
 */
static unsigned long __init zone_spanned_pages_in_node(int nid,
					unsigned long zone_type,
					unsigned long node_start_pfn,
					unsigned long node_end_pfn,
					unsigned long *zone_start_pfn,
					unsigned long *zone_end_pfn,
					unsigned long *ignored)
{
	/* When hotadd a new node from cpu_up(), the node should be empty */
	if (!node_start_pfn && !node_end_pfn)
		return 0;

	*zone_start_pfn = arch_zone_lowest_possible_pfn[zone_type];
	*zone_end_pfn = arch_zone_highest_possible_pfn[zone_type];

	/* Check that this node has pages within the zone's required range */
	if (*zone_end_pfn < node_start_pfn || *zone_start_pfn > node_end_pfn)
		return 0;

	/* Move the zone boundaries inside the node if necessary */
	*zone_end_pfn = min(*zone_end_pfn, node_end_pfn);
	*zone_start_pfn = max(*zone_start_pfn, node_start_pfn);

	return *zone_end_pfn - *zone_start_pfn;
}

/*
 * Return the number of holes in a range on a node. If nid is MAX_NR_NODES,
 * then all holes in the requested range will be accounted for.
 */
unsigned long __init __absent_pages_in_range(int nid,
				unsigned long range_start_pfn,
				unsigned long range_end_pfn)
{
	unsigned long nr_absent = range_end_pfn - range_start_pfn;
	unsigned long start_pfn, end_pfn;
	int i;

	for_each_mem_pfn_range(i, nid, &start_pfn, &end_pfn, NULL) {
		start_pfn = clamp(start_pfn, range_start_pfn, range_end_pfn);
		end_pfn = clamp(end_pfn, range_start_pfn, range_end_pfn);
		nr_absent -= end_pfn - start_pfn;
	}
	return nr_absent;
}

/**
 * absent_pages_in_range - Return number of page frames in holes within a range
 * @start_pfn: The start PFN to start searching for holes
 * @end_pfn: The end PFN to stop searching for holes
 *
 * It returns the number of pages frames in memory holes within a range.
 */
unsigned long __init absent_pages_in_range(unsigned long start_pfn,
					   unsigned long end_pfn)
{
	return __absent_pages_in_range(MAX_NR_NODES, start_pfn, end_pfn);
}

/* Return the number of page frames in holes in a zone on a node */
static unsigned long __init zone_absent_pages_in_node(int nid,
					unsigned long zone_type,
					unsigned long node_start_pfn,
					unsigned long node_end_pfn,
					unsigned long *ignored)
{
	unsigned long zone_low = arch_zone_lowest_possible_pfn[zone_type];
	unsigned long zone_high = arch_zone_highest_possible_pfn[zone_type];
	unsigned long zone_start_pfn, zone_end_pfn;
	unsigned long nr_absent;

	/* When hotadd a new node from cpu_up(), the node should be empty */
	if (!node_start_pfn && !node_end_pfn)
		return 0;

	zone_start_pfn = clamp(node_start_pfn, zone_low, zone_high);
	zone_end_pfn = clamp(node_end_pfn, zone_low, zone_high);

	nr_absent = __absent_pages_in_range(nid, zone_start_pfn, zone_end_pfn);

	return nr_absent;
}

static void __init calculate_node_totalpages(struct pglist_data *pgdata,
					     unsigned long node_start_pfn,
					     unsigned long node_end_pfn,
					     unsigned long *zones_size,
					     unsigned long *zholes_size)
{
	unsigned long realtotalpages = 0, totalpages = 0;
	int i;

	for (i = 0; i < __MAX_NR_ZONES; i++) {
		struct zone *zone  = pgdata->node_zones + i;
		unsigned long zone_start_pfn, zone_end_pfn;
		unsigned long size, real_size;

		size = zone_spanned_pages_in_node(pgdata->node_id, i,
						  node_start_pfn,
						  node_end_pfn,
						  &zone_start_pfn,
						  &zone_end_pfn,
						  zones_size);

		real_size = size - zone_absent_pages_in_node(pgdata->node_id, i,
						  node_start_pfn, node_end_pfn,
						  zholes_size);
		if (size)
			zone->zone_start_pfn = zone_start_pfn;
		else
			zone->zone_start_pfn = 0;
		zone->spanned_pages = size;
		zone->present_pages = real_size;

		totalpages += size;
		realtotalpages += real_size;
	}

	pgdata->node_spanned_pages = totalpages;
	pgdata->node_present_pages = realtotalpages;

	pr_debug("On node %d spanned_pages: %lu present_pages: %lu\n",
		pgdata->node_id, totalpages, realtotalpages);
}

/**
 * get_pfn_range_for_nid - Return the start and end page frames for a node
 * @nid: The nid to return the range for. If MAX_NR_NODES, the min and max PFN are returned.
 * @start_pfn: Passed by reference. On return, it will have the node start_pfn.
 * @end_pfn: Passed by reference. On return, it will have the node end_pfn.
 *
 * It returns the start and end page frame of a node based on information
 * provided by memblock_set_node(). If called for a node
 * with no available memory, a warning is printed and the start and end
 * PFNs will be 0.
 */
void __init get_pfn_range_for_nid(unsigned int nid, unsigned long *start_pfn,
				  unsigned long *end_pfn)
{
	unsigned long this_start_pfn, this_end_pfn;
	int i;

	*start_pfn = -1UL;
	*end_pfn = 0;

	for_each_mem_pfn_range(i, nid, &this_start_pfn, &this_end_pfn, NULL) {
		*start_pfn = min(*start_pfn, this_start_pfn);
		*end_pfn = max(*end_pfn, this_end_pfn);
	}

	if (*start_pfn == -1UL)
		*start_pfn = 0;
}

static inline int IS_HIGHMEM_IDX(enum ZONE_TYPE idx)
{
#ifdef CONFIG_HIGHMEM
	return idx == ZONE_HIGHMEM;
#else
	return 0;
#endif
}

/*
 * Set up the zone data structures:
 *   - mark all pages reserved
 *   - mark all memory queues empty
 *   - clear the memory bitmaps
 *
 * NOTE: pgdata should get zeroed by caller.
 */
static void __init free_area_init_core(struct pglist_data *pgdata)
{
	int i;

	for (i = 0; i < __MAX_NR_ZONES; i++) {
		struct zone *zone = pgdata->node_zones + i;
		unsigned long memmap_pages;
		unsigned long size, realsize, freesize;

		size = zone->spanned_pages;
		realsize = freesize = zone->present_pages;

		/*
		 * Adjust freesize so that it accounts for how much memory
		 * is used by this zone for memmap. This affects the watermark
		 * and per-cpu initialisations
		 */
		memmap_pages = PAGE_ALIGN(zone->spanned_pages * sizeof(struct page)) >> PAGE_SHIFT;
		if (!IS_HIGHMEM_IDX(i)){
			if (freesize >= memmap_pages) {
				freesize -= memmap_pages;
				if (memmap_pages)
					pr_debug( "  %s zone: %lu pages used for memmap\n",
						zone_names[i], memmap_pages);
			} else {
				pr_warn("  %s zone: %lu pages exceeds freesize %lu\n",
					zone_names[i], memmap_pages, freesize);
			}
		}

		/* Account for reserved pages */
		if (i == 0 && freesize > dma_reserve) {
			freesize -= dma_reserve;
			pr_debug("  %s zone: %lu pages reserved\n",
				zone_names[0], dma_reserve);
		}

		if (!IS_HIGHMEM_IDX(i))
			nr_kernel_pages += freesize;
		/* Charge for highmem memmap if there are enough kernel pages */
		else if (nr_kernel_pages > memmap_pages * 2)
			nr_kernel_pages -= memmap_pages;
		nr_all_pages += freesize;

		/*
		 * Set an approximate value for lowmem here, it will be adjusted
		 * when the bootmem allocator frees pages into the buddy system.
		 * And all highmem pages will be managed by the buddy system.
		 */
		zone->managed_pages = IS_HIGHMEM_IDX(i) ? realsize : freesize;
#ifdef CONFIG_NUMA
		zone->node = pgdata->node_id;
#endif
		zone->name = zone_names[i];
		zone->zone_pgdata = pgdata;

		/*
		 * TODO: Not finished
		 */
	}
}

/*
 * Allocate mem_map array for memory node
 */
static void __init alloc_node_mem_map(struct pglist_data *pgdata)
{
	unsigned long __maybe_unused start = 0;
	unsigned long __maybe_unused offset = 0;
	unsigned long __maybe_unused size = 0;

	if (!pgdata->node_spanned_pages)
		return;

/*
 * CONFIG_FLAT_NODE_MEM_MAP means the memory model is FLATMEM or DISCONTGMEM
 * FLATMEM only has one pg_data_t and one mem_map, DISCONTGMEM has mem_map
 * per pg_data_t.
 *
 * SPARSEMEM use mem_section to calculate pfn_to_page or page_to_pfn.
 *
 * Check <asm-generic/memory_model.h> for detail.
 */
#ifdef CONFIG_FLAT_NODE_MEM_MAP
	start = pgdata->node_start_pfn & ~(MAX_ORDER_NR_PAGES - 1);
	offset = pgdata->node_start_pfn - start;

	if (!pgdata->node_mem_map) {
		unsigned long end;
		struct page *map;

		end = pgdata_end_pfn(pgdata);
		end = ALIGN(end, MAX_ORDER_NR_PAGES);
		size = (end - start) * sizeof(struct page);

		map = memblock_virt_alloc_node_nopanic(size, pgdata->node_id);

		pgdata->node_mem_map = map + offset;
	}

	pr_debug("%s: node %d, pgdata %#08lx, node_mem_map %#08lx (%lu pages)\n",
		__func__, pgdata->node_id, (unsigned long)pgdata,
		(unsigned long)pgdata->node_mem_map, (unsigned long)(size >> PAGE_SHIFT));

#ifndef CONFIG_NEED_MULTIPLE_NODES
	/*
	 * In FLATMEM, there is a global mem_map and it is the same as node 0's
	 */
	if (pgdata == NODE_DATA(0)) {
		WARN_ON(mem_map);
		mem_map = pgdata->node_mem_map;
#if defined(CONFIG_HAVE_MEMBLOCK_NODE_MAP) || defined(CONFIG_FLATMEM)
		if (page_to_pfn(mem_map) != pgdata->node_start_pfn)
			mem_map -= offset;
#endif /* CONFIG_HAVE_MEMBLOCK_NODE_MAP */
	}
#endif /* CONFIG_NEED_MULTIPLE_NODES */

#endif /* CONFIG_FLAT_NODE_MEM_MAP */
}

/**
 * free_area_init_node - Initialize a single pg_data_t and zone data
 */
void __init free_area_init_node(int nid, unsigned long *zones_size,
				unsigned long node_start_pfn,
				unsigned long *zholes_size)
{

	pg_data_t *pgdata = NODE_DATA(nid);
	unsigned long start_pfn = 0;
	unsigned long end_pfn = 0;

	pgdata->node_id = nid;
	pgdata->node_start_pfn = node_start_pfn;

#ifdef CONFIG_HAVE_MEMBLOCK_NODE_MAP
	get_pfn_range_for_nid(nid, &start_pfn, &end_pfn);
	pr_info("Initmem setup node %d [mem %#018Lx-%#018Lx]\n", nid,
		(u64)start_pfn << PAGE_SHIFT,
		end_pfn ? ((u64)end_pfn << PAGE_SHIFT) - 1 : 0);
#else
	start_pfn = node_start_pfn;
#endif

	calculate_node_totalpages(pgdata, start_pfn, end_pfn,
				  zones_size, zholes_size);

	/*
	 * Allocate the so-called mem_map of struct page, if we are
	 * configured as FLATMEM or DISCONTIGMEM
	 */
	alloc_node_mem_map(pgdata);

	free_area_init_core(pgdata);
}

/**
 * free_area_init_nodes - Initialize all pg_data_t and zone data
 * @zone_max_pfns: an array of max PFNs for each zone
 *
 * This will call free_area_init_node() for each active node in the system.
 * Using the page ranges provided by memblock_set_node(), the size of each
 * zone in each node and their holes is calculated. If the maximum PFN
 * between two adjacent zones match, it is assumed that the zone is empty.
 * For example, if arch_max_dma_pfn == arch_max_dma32_pfn, it is assumed
 * that arch_max_dma32_pfn has no pages. It is also assumed that a zone
 * starts where the previous one ended. For example, ZONE_DMA32 starts
 * at arch_max_dma_pfn.
 */
void __init free_area_init_nodes(unsigned long *zone_max_pfns)
{
	unsigned long start_pfn, end_pfn;
	int i, nid;

	memset(arch_zone_lowest_possible_pfn, 0, sizeof(arch_zone_lowest_possible_pfn));
	memset(arch_zone_highest_possible_pfn, 0, sizeof(arch_zone_highest_possible_pfn));

	arch_zone_lowest_possible_pfn[0] = find_min_pfn_with_active_regions();
	arch_zone_highest_possible_pfn[0] = zone_max_pfns[0];

	for (i = 1; i < __MAX_NR_ZONES; i++) {
		if (i == ZONE_MOVABLE)
			continue;
		arch_zone_lowest_possible_pfn[i] =
			arch_zone_highest_possible_pfn[i - 1];
		arch_zone_highest_possible_pfn[i] =
			max(zone_max_pfns[i], arch_zone_lowest_possible_pfn[i]);
	}

	/*
	 * Find PFNs that ZONE_MOVABLE begins at in each node:
	 */
	arch_zone_lowest_possible_pfn[ZONE_MOVABLE] = 0;
	arch_zone_highest_possible_pfn[ZONE_MOVABLE] = 0;
	memset(zone_movable_pfn, 0, sizeof(zone_movable_pfn));
	find_zone_movable_pfns_for_nodes();

	/* Print out the zone ranges */
	pr_info("Zone ranges:\n");
	for (i = 0; i < __MAX_NR_ZONES; i++) {
		if (i == ZONE_MOVABLE)
			continue;
		pr_info("  %-8s ", zone_names[i]);
		if (arch_zone_lowest_possible_pfn[i] ==
				arch_zone_highest_possible_pfn[i])
			pr_cont("empty\n");
		else
			pr_cont("[mem %#018Lx-%#018Lx]\n",
				(u64)arch_zone_lowest_possible_pfn[i]
					<< PAGE_SHIFT,
				((u64)arch_zone_highest_possible_pfn[i]
					<< PAGE_SHIFT) - 1);
	}

	/* Print out the PFNs ZONE_MOVABLE begins at in each node */
	pr_info("Movable zone start for each node\n");
	for (i = 0; i < MAX_NR_NODES; i++) {
		if (zone_movable_pfn[i])
			pr_info("  Node %d: %#018Lx\n", i,
			       (u64)zone_movable_pfn[i] << PAGE_SHIFT);
	}

	/* Print out the early node map */
	pr_info("Early memory node ranges\n");
	for_each_mem_pfn_range(i, MAX_NR_NODES, &start_pfn, &end_pfn, &nid)
		pr_info("  node %3d: [mem %#018Lx-%#018Lx]\n", nid,
			(u64)start_pfn << PAGE_SHIFT,
			((u64)end_pfn << PAGE_SHIFT) - 1);

	setup_nr_node_ids();

	for_each_online_node(nid) {
		pg_data_t *pgdata = NODE_DATA(nid);

		free_area_init_node(nid, NULL,
				find_min_pfn_for_node(nid), NULL);

		if (pgdata->node_present_pages)
			node_set_state(nid, NODE_MEMORY);
	}
}
