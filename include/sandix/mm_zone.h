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

#include <sandix/numa.h>
#include <sandix/list.h>
#include <sandix/kernel.h>
#include <sandix/spinlock.h>

#include <asm-generic/memory_model.h>

#define MAX_ORDER		11
#define MAX_ORDER_NR_PAGES	(1 << (MAX_ORDER - 1))

enum MIGRATE_TYPES {
	MIGRATE_UNMOVABLE,
	MIGRATE_MOVABLE,
	MIGRATE_RECLAIMABLE,
	MIGRATE_PCPTYPES,	/* nr of types on the pcp lists */
	MIGRATE_HIGHATOMIC = MIGRATE_PCPTYPES,

	__MAX_NR_MIGRATE_TYPES
};

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

enum ZONE_FLAGS {
	ZONE_RECLAIM_LOCKED,		/* prevents concurrent reclaim */
	ZONE_OOM_LOCKED,		/* zone is in OOM killer zonelist */
	ZONE_CONGESTED,			/* zone has many dirty pages backed by
					 * a congested BDI
					 */
	ZONE_DIRTY,			/* reclaim scanning has recently found
					 * many dirty file pages at the tail
					 * of the LRU.
					 */
	ZONE_WRITEBACK,			/* reclaim scanning has recently found
					 * many pages under writeback
					 */
	ZONE_FAIR_DEPLETED,		/* fair zone policy batch depleted */

	__MAX_NR_ZONE_FLAGS
};

enum ZONE_WATERMARKS {
	WATERMARK_MIN,
	WATERMARK_LOW,
	WATERMARK_HIGH,

	__MAX_NR_WATERMARK
};

#define min_watermark_pages(z)	(z->watermark[WATERMARK_MIN])
#define low_watermark_pages(z)	(z->watermark[WATERMARK_LOW])
#define high_watermark_pages(z)	(z->watermark[WATERMARK_HIGH])

struct free_area {
	struct list_head	free_list[__MAX_NR_MIGRATE_TYPES];
	unsigned long		nr_free;
};

struct pglist_data;

struct zone {
	/*
	 * zone watermarks, access with *_watermark_pages(z) macros
	 */
	unsigned long		watermark[__MAX_NR_WATERMARK];

#ifdef CONFIG_NUMA
	int			node;
#endif

	/*
	 * The target ration of ACTIVE_ANON to INACTIVE_ANON pages on
	 * this zone's LRU. Maintained by the pageout code.
	 */
	unsigned int		inactive_ratio;

	/*
	 * The Memory Node:
	 */
	struct pglist_data	*zone_pgdata;

	/*
	 * This is a per-zone reserve of pages that are not available
	 * to userspace allocations.
	 */
	unsigned long		totalreserve_pages;

#ifndef CONFIG_SPARSEMEM
	/*
	 * Flags for a pageblock_nr_pages block. See pageblock-flags.h.
	 * In SPARSEMEM, this map is stored in struct mem_section
	 */
	unsigned long		*pageblock_flags;
#endif

	/*
	 * zone_start_pfn == zone_start_paddr >> PAGE_SHIFT
	 *
	 * spanned_pages is the total pages spanned by the zone, including
	 * holes, which is calculated as:
	 * 	spanned_pages = zone_end_pfn - zone_start_pfn;
	 *
	 * present_pages is physical pages existing within the zone, which
	 * is calculated as:
	 *	present_pages = spanned_pages - absent_pages(pages in holes);
	 *
	 * managed_pages is present pages managed by the buddy system, which
	 * is calculated as (reserved_pages includes pages allocated by the
	 * bootmem allocator):
	 *	managed_pages = present_pages - reserved_pages;
	 *
	 * So present_pages may be used by memory hotplug or memory power
	 * management logic to figure out unmanaged pages by checking
	 * (present_pages - managed_pages). And managed_pages should be used
	 * by page allocator and vm scanner to calculate all kinds of watermarks
	 * and thresholds.
	 *
	 * Locking rules:
	 *
	 * zone_start_pfn and spanned_pages are protected by span_seqlock.
	 * It is a seqlock because it has to be read outside of zone->lock,
	 * and it is done in the main allocator path.  But, it is written
	 * quite infrequently.
	 *
	 * The span_seq lock is declared along with zone->lock because it is
	 * frequently read in proximity to zone->lock.  It's good to
	 * give them a chance of being in the same cacheline.
	 *
	 * Write access to present_pages at runtime should be protected by
	 * mem_hotplug_begin/end(). Any reader who can't tolerant drift of
	 * present_pages should get_online_mems() to get a stable value.
	 *
	 * Read access to managed_pages should be safe because it's unsigned
	 * long. Write access to zone->managed_pages and totalram_pages are
	 * protected by managed_page_count_lock at runtime. Idealy only
	 * adjust_managed_page_count() should be used instead of directly
	 * touching zone->managed_pages and totalram_pages.
	 */
	unsigned long		zone_start_pfn;
	unsigned long		managed_pages;
	unsigned long		spanned_pages;
	unsigned long		present_pages;

	const char		*name;

#ifdef CONFIG_MEMORY_HOTPLUG
	seqlock_t		span_seqlock;
#endif

	/*
	 * This field identifies the blocks of free page frames in the zone
	 */
	struct free_area	free_area[MAX_ORDER];

	/*
	 * zone flags, see ZONE_FLAGS above
	 */
	unsigned long		flags;

	/*
	 * Write-intensive fields used from the page allocator
	 */
	spinlock_t		lock;

} __cacheline_aligned;

/**
 * zone_end_pfn		-	Return the last pfn this zone spans
 * @zone: zone in question
 * @return: the end pfn
 */
static inline unsigned long zone_end_pfn(const struct zone *zone)
{
	return zone->zone_start_pfn + zone->spanned_pages;
}

/**
 * zone_spans_pfn	-	Check if pfn belongs to a zone
 * @zone: zone in question
 * @pfn: the pfn to check
 *
 * Returns TRUE if this @pfn is spanned by this @zone
 */
static inline bool zone_spans_pfn(const struct zone *zone, unsigned long pfn)
{
	return zone->zone_start_pfn <= pfn && pfn < zone_end_pfn(zone);
}

static inline bool zone_is_initialized(struct zone *zone)
{
	return true;
}

static inline bool zone_is_empty(const struct zone *zone)
{
	return zone->spanned_pages == 0;
}

/* Maximum number of zones on a zonelist */
#define MAX_ZONES_PER_ZONELIST	(MAX_NR_NODES * __MAX_NR_ZONES)

enum {
	ZONELIST_FALLBACK,	/* zonelist with fallback */
#ifdef CONFIG_NUMA
	/*
	 * The NUMA zonelists are doubled because we need zonelists that
	 * restrict the allocations to a single node for __GFP_THISNODE.
	 */
	ZONELIST_NOFALLBACK,	/* zonelist without fallback (__GFP_THISNODE) */
#endif

	__MAX_NR_ZONELISTS
};

/*
 * This struct contains information about a zone in a zonelist. It is stored
 * here to avoid dereferences into large structures and lookups of tables
 */
struct zoneref {
	struct zone *zone;	/* Pointer to actual zone */
	int zone_idx;		/* zone_idx(zoneref->zone) */
};

/*
 * One allocation request operates on a zonelist. A zonelist
 * is a list of zones, the first one is the 'goal' of the
 * allocation, the other zones are fallback zones, in decreasing
 * priority.
 *
 * To speed the reading of the zonelist, the zonerefs contain the zone index
 * of the entry being read. Helper functions to access information given
 * a struct zoneref are
 *
 * zonelist_zone()	- Return the struct zone * for an entry in _zonerefs
 * zonelist_zone_idx()	- Return the index of the zone for an entry
 * zonelist_node_idx()	- Return the index of the node for an entry
 */
struct zonelist {
	struct zoneref _zonerefs[MAX_ZONES_PER_ZONELIST + 1];
};

/*
 * The array of struct pages.
 *
 * For discontigmem use pgdata->node_mem_map
 * For sparsemem use mem_section.
 */
#ifdef CONFIG_FLATMEM
extern struct page *mem_map;
#endif

/*
 * The pg_data_t structure is used in machines with CONFIG_DISCONTIGMEM
 * (mostly NUMA machines?) to denote a higher-level memory zone than the
 * zone denotes.
 *
 * On NUMA machines, each NUMA node would have a pg_data_t to describe
 * it's memory layout.
 *
 * Memory statistics and page replacement data structures are maintained on a
 * per-zone basis.
 */
typedef struct pglist_data {
	struct zone		node_zones[__MAX_NR_ZONES];
	struct zonelist		node_zonelists[__MAX_NR_ZONELISTS];
	int			nr_zones;

#ifdef CONFIG_FLAT_NODE_MEM_MAP /* means !SPARSEMEM */
	struct page		*node_mem_map;
#endif

	/*
	 * node_present_pages is size of the memory node,
	 * excluding holes (in page frames)
	 *
	 * node_spanned_pages is size of the node,
	 * including holes (in page frames)
	 */
	unsigned long		node_start_pfn;
	unsigned long		node_present_pages;
	unsigned long		node_spanned_pages;

	int			node_id;
} pg_data_t;

static inline unsigned long pgdata_end_pfn(pg_data_t *pgdata)
{
	return pgdata->node_start_pfn + pgdata->node_spanned_pages;
}

static inline bool pgdata_is_empty(pg_data_t *pgdata)
{
	return !pgdata->node_start_pfn && !pgdata->node_spanned_pages;
}

static inline int zond_id(const struct zone *zone)
{
	struct pglist_data *pgdata = zone->zone_pgdata;

	return zone - pgdata->node_zones;
}

/*
 * If NUMA is configured into kernel, then all pg_data_t descriptors
 * are arranged into an array, which is defined in arch/~/mm/numa.c
 * and declared in <asm/mm_zone.h>
 *
 * Otherwise, a single pg_data_t is used, which is defined
 * in arch/~/mm/init.c and declared here.
 */
#ifndef CONFIG_NEED_MULTIPLE_NODES
extern pg_data_t contiguous_page_data;
#define NODE_DATA(nid)		(&contiguous_page_data)
#define NODE_MEM_MAP(nid)	mem_map
#else
#include <asm/mm_zone.h>
#endif /* CONFIG_NEED_MULTIPLE_NODES */

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
