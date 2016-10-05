/*
 *	Copyright (C) 2015-2016 Yizhou Shan <shan13@purdue.edu>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 */

#include <asm/dma.h>
#include <asm/e820.h>
#include <asm/page.h>
#include <asm/setup.h>
#include <asm/pgtable.h>
#include <asm/sections.h>
#include <asm/tlbflush.h>
#include <asm/cpufeature.h>

#include <sandix/mm.h>
#include <sandix/mm_zone.h>
#include <sandix/pfn.h>
#include <sandix/numa.h>
#include <sandix/export.h>
#include <sandix/kernel.h>
#include <sandix/printk.h>
#include <sandix/string.h>
#include <sandix/kconfig.h>
#include <sandix/memblock.h>

#include "mm_internal.h"

/*
 * The single pg_data_t for non-NUMA configured kernel.
 * The array list of pg_data_t is used only if NUMA or DISCONTIGMEM enabled.
 */
#ifndef CONFIG_NEED_MULTIPLE_NODE
struct pglist_data contiguous_page_data __read_mostly;
EXPORT_SYMBOL(contiguous_page_data);
#endif

/*
 * Tables translating between page_cache_type_t and pte encoding.
 *
 * The default values are defined statically as minimal supported mode;
 * WC and WT fall back to UC-.  pat_init() updates these values to support
 * more cache modes, WC and WT, when it is safe to do so.  See pat_init()
 * for the details.  Note, __early_ioremap() used during early boot-time
 * takes pgprot_t (pte encoding) and does not use these tables.
 *
 *   Index into __cachemode2pte_tbl[] is the cachemode.
 *
 *   Index into __pte2cachemode_tbl[] are the caching attribute bits of the pte
 *   (_PAGE_PWT, _PAGE_PCD, _PAGE_PAT) at index bit positions 0, 1, 2.
 */
u16 __cachemode2pte_tbl[__PAGE_CACHE_MODE_NUM] = {
	[__PAGE_CACHE_MODE_WB      ]	= 0          | 0         ,
	[__PAGE_CACHE_MODE_WC      ]	= 0          | __PAGE_PCD,
	[__PAGE_CACHE_MODE_UC_MINUS]	= 0          | __PAGE_PCD,
	[__PAGE_CACHE_MODE_UC      ]	= __PAGE_PWT | __PAGE_PCD,
	[__PAGE_CACHE_MODE_WT      ]	= 0          | __PAGE_PCD,
	[__PAGE_CACHE_MODE_WP      ]	= 0          | __PAGE_PCD,
};
EXPORT_SYMBOL(__cachemode2pte_tbl);

u8 __pte2cachemode_tbl[8] = {
	[__pte2cm_idx( 0         | 0          | 0         )] = __PAGE_CACHE_MODE_WB,
	[__pte2cm_idx(__PAGE_PWT | 0          | 0         )] = __PAGE_CACHE_MODE_UC_MINUS,
	[__pte2cm_idx( 0         | __PAGE_PCD | 0         )] = __PAGE_CACHE_MODE_UC_MINUS,
	[__pte2cm_idx(__PAGE_PWT | __PAGE_PCD | 0         )] = __PAGE_CACHE_MODE_UC,
	[__pte2cm_idx( 0         | 0          | __PAGE_PAT)] = __PAGE_CACHE_MODE_WB,
	[__pte2cm_idx(__PAGE_PWT | 0          | __PAGE_PAT)] = __PAGE_CACHE_MODE_UC_MINUS,
	[__pte2cm_idx(0          | __PAGE_PCD | __PAGE_PAT)] = __PAGE_CACHE_MODE_UC_MINUS,
	[__pte2cm_idx(__PAGE_PWT | __PAGE_PCD | __PAGE_PAT)] = __PAGE_CACHE_MODE_UC,
};
EXPORT_SYMBOL(__pte2cachemode_tbl);

static unsigned int page_size_mask;

static void __init probe_page_size_mask(void)
{
	if (cpu_has_pse) {
		/* need to write cr4 to enable PSE */
		pr_info("PSE is available\n");
		page_size_mask |= 1 << PG_LEVEL_2M;
	}

	if (cpu_has_pge)
		__supported_pte_mask |= __PAGE_GLOBAL;
	else
		__supported_pte_mask &= ~__PAGE_GLOBAL;

	if (cpu_has_gbpages) {
		pr_info("GB Pages is available\n");
		page_size_mask |= 1 << PG_LEVEL_1G;
	}
}

/*
 * Early page table buffers, reserved from BRK:
 *    3 4k pages for initial PMD_SIZE
 *    3 4k pages for 0-ISA_END_ADDRESS
 *
 * But please notice that no matter how many pages we reserve, it will
 * not possible to reserve so many page tables. So new pages must be
 * allocated from already direct mapped pages (see alloc_low_pages()).
 */
#define INIT_PGT_BUF_SIZE	(6 * PAGE_SIZE)
static unsigned long __initdata pgt_buf_start;
static unsigned long __initdata pgt_buf_end;
static unsigned long __initdata pgt_buf_top;

static bool __initdata can_use_brk_pgt = true;

static unsigned long min_pfn_mapped;

void __init early_alloc_pgt_buf(void)
{
	unsigned long tables = INIT_PGT_BUF_SIZE;
	phys_addr_t base;

	base = __pa(extend_brk(tables, PAGE_SIZE));

	pgt_buf_start = base >> PAGE_SHIFT;
	pgt_buf_end = pgt_buf_start;
	pgt_buf_top = pgt_buf_start + (tables >> PAGE_SHIFT);
}

/**
 * alloc_low_pages
 * @num: number of pages to allocate
 *
 * The big FAT note is: pages returned are already directly mapped.
 * Since those pages are used as page tables for now.
 */
void * __init alloc_low_pages(unsigned long num)
{
	unsigned long pfn;
	int i;

	if ((pgt_buf_end + num) > pgt_buf_top || !can_use_brk_pgt) {
		unsigned long ret;

		if (min_pfn_mapped >= max_pfn_mapped)
			panic("OOM: min_pfn_mapped: %lu, max_pfn_mapped: %lu",
				min_pfn_mapped, max_pfn_mapped);

		ret = memblock_find_in_range(min_pfn_mapped << PAGE_SHIFT,
					     max_pfn_mapped << PAGE_SHIFT,
					     PAGE_SIZE * num, PAGE_SIZE);
		if (!ret)
			panic("fail to alloc memory");

		memblock_reserve(ret, PAGE_SIZE * num);
		pfn = ret >> PAGE_SHIFT;
	} else {
		pfn = pgt_buf_end;
		pgt_buf_end += num;
		pr_debug("BRK [%#010lx, %#010lx] PGTABLE\n",
			pfn << PAGE_SHIFT, (pgt_buf_end << PAGE_SHIFT) - 1);
	}

	for (i = 0; i < num; i++) {
		void *address;

		address = __va((pfn + i) << PAGE_SHIFT);
		memset(address, 0, PAGE_SIZE);
	}

	return __va(pfn << PAGE_SHIFT);
}

/*
 * Memory Ranges is used to contain different page types, e.g. 4KB, 2MB, 1GB
 * Kernel should use GB mapping if possible since this will be more efficient.
 * (It reminds to read some TLB, MMU, OS related papers, by the way).
 *
 * Again, for now, Sandix use 4KB only.
 */
#ifdef CONFIG_X86_32
#define NR_MR_RANGE	3
#else
#define NR_MR_RANGE	5
#endif

struct map_range {
	unsigned long start;
	unsigned long end;
	unsigned int page_size_mask;
};

static const char *page_size_string(struct map_range *mr)
{
	static const char str_1g[] = "1G";
	static const char str_2m[] = "2M";
	static const char str_4m[] = "4M";
	static const char str_4k[] = "4k";

	if (mr->page_size_mask & (1<<PG_LEVEL_1G))
		return str_1g;

	/*
	 * 32-bit without PAE has a 4M large page size.
	 * PG_LEVEL_2M is misnamed, but we can at least
	 * print out the right size in the string.
	 */
	if (IS_ENABLED(CONFIG_X86_32) &&
	    !IS_ENABLED(CONFIG_X86_PAE) &&
	    mr->page_size_mask & (1<<PG_LEVEL_2M))
		return str_4m;

	if (mr->page_size_mask & (1<<PG_LEVEL_2M))
		return str_2m;

	return str_4k;
}

static int save_mr(struct map_range *mr, int nr_range,
		   unsigned long start_pfn, unsigned long end_pfn,
		   unsigned long page_size_mask)
{
	if (start_pfn < end_pfn) {
		if (nr_range >= NR_MR_RANGE)
			panic("run out of range for init_memory_mapping");
		mr[nr_range].start = start_pfn << PAGE_SHIFT;
		mr[nr_range].end = end_pfn << PAGE_SHIFT;
		mr[nr_range].page_size_mask = page_size_mask;
		nr_range++;
	}

	return nr_range;
}

static int split_mem_range(struct map_range *mr, int nr_range,
			   unsigned long start, unsigned long end)
{
/*
 * Note that:
 * Linux use 4K, 2M, 1G mapping if possible. So linux needs to split the
 * address range into these 3 types. Hmm, for Sandix case, I decided to use
 * 4KB first. But remember how other people achieve this.
 */
#if 0
	unsigned long start_pfn, end_pfn, limit_pfn;
	unsigned long pfn;
	int i;

	limit_pfn = PFN_DOWN(end);

	/* head if not big page alignment ? */
	pfn = start_pfn = PFN_DOWN(start);
#ifdef CONFIG_X86_32
	/*
	 * Don't use a large page for the first 2/4MB of memory
	 * because there are often fixed size MTRRs in there
	 * and overlapping MTRRs into large pages can cause
	 * slowdowns.
	 */
	if (pfn == 0)
		end_pfn = PFN_DOWN(PMD_SIZE);
	else
		end_pfn = round_up(pfn, PFN_DOWN(PMD_SIZE));
#else
	end_pfn = round_up(pfn, PFN_DOWN(PMD_SIZE));
#endif
	if (end_pfn > limit_pfn)
		end_pfn = limit_pfn;
	if (start_pfn < end_pfn) {
		nr_range = save_mr(mr, nr_range, start_pfn, end_pfn, 0);
		pfn = end_pfn;
	}

	/* big page (2M) range */
	start_pfn = round_up(pfn, PFN_DOWN(PMD_SIZE));
#ifdef CONFIG_X86_32
	end_pfn = round_down(limit_pfn, PFN_DOWN(PMD_SIZE));
#else /* CONFIG_X86_64 */
	end_pfn = round_up(pfn, PFN_DOWN(PUD_SIZE));
	if (end_pfn > round_down(limit_pfn, PFN_DOWN(PMD_SIZE)))
		end_pfn = round_down(limit_pfn, PFN_DOWN(PMD_SIZE));
#endif

	if (start_pfn < end_pfn) {
		nr_range = save_mr(mr, nr_range, start_pfn, end_pfn,
				page_size_mask & (1<<PG_LEVEL_2M));
		pfn = end_pfn;
	}

#ifdef CONFIG_X86_64
	/* big page (1G) range */
	start_pfn = round_up(pfn, PFN_DOWN(PUD_SIZE));
	end_pfn = round_down(limit_pfn, PFN_DOWN(PUD_SIZE));
	if (start_pfn < end_pfn) {
		nr_range = save_mr(mr, nr_range, start_pfn, end_pfn,
				page_size_mask &
				 ((1<<PG_LEVEL_2M)|(1<<PG_LEVEL_1G)));
		pfn = end_pfn;
	}

	/* tail is not big page (1G) alignment */
	start_pfn = round_up(pfn, PFN_DOWN(PMD_SIZE));
	end_pfn = round_down(limit_pfn, PFN_DOWN(PMD_SIZE));
	if (start_pfn < end_pfn) {
		nr_range = save_mr(mr, nr_range, start_pfn, end_pfn,
				page_size_mask & (1<<PG_LEVEL_2M));
		pfn = end_pfn;
	}
#endif

	/* tail is not big page (2M) alignment */
	start_pfn = pfn;
	end_pfn = limit_pfn;
	nr_range = save_mr(mr, nr_range, start_pfn, end_pfn, 0);

	if (!after_bootmem)
		adjust_range_page_size_mask(mr, nr_range);

	/* try to merge same page size and continuous */
	for (i = 0; nr_range > 1 && i < nr_range - 1; i++) {
		unsigned long old_start;
		if (mr[i].end != mr[i+1].start ||
		    mr[i].page_size_mask != mr[i+1].page_size_mask)
			continue;
		/* move it */
		old_start = mr[i].start;
		memmove(&mr[i], &mr[i+1],
			(nr_range - 1 - i) * sizeof(struct map_range));
		mr[i--].start = old_start;
		nr_range--;
	}
#endif

	unsigned long start_pfn, end_pfn;
	int i;

	start_pfn = PFN_DOWN(start);
	end_pfn = PFN_UP(end);
	nr_range = save_mr(mr, nr_range, start_pfn, end_pfn, 0);

	for (i = 0; i < nr_range; i++)
		pr_debug(" [mem %#010lx-%#010lx] page %s\n",
				mr[i].start, mr[i].end - 1,
				page_size_string(&mr[i]));

	return nr_range;
}

/*
 * add_pfn_range_mapped	-	Update mapped pfn
 *
 * Bookkeeping about the mapping information of [page frame number],
 * both max_pfn_mapped and max_low_pfn_mapped will be updated here:
 */
static void add_pfn_range_mapped(unsigned long start_pfn, unsigned long end_pfn)
{
	max_pfn_mapped = max(max_pfn_mapped, end_pfn);

	if (start_pfn < (1UL << (32 - PAGE_SHIFT)))
		max_low_pfn_mapped = max(max_low_pfn_mapped,
					 min(end_pfn, 1UL << (32 - PAGE_SHIFT)));
}

/*
 * Setup the direct mapping of the physical memory at PAGE_OFFSET.
 * This runs before bootmem is initialized and gets pages directly from
 * the physical memory. To access them they are temporarily mapped.
 */
unsigned long __init init_memory_mapping(unsigned long start,
					 unsigned long end)
{
	struct map_range mr[NR_MR_RANGE];
	int nr_range, i;
	unsigned long ret = 0;

	pr_debug("init_memory_mapping: [mem %#010lx-%#010lx]\n",
		start, end - 1);

	memset(mr, 0, sizeof(mr));
	nr_range = split_mem_range(mr, 0, start, end);

	for (i = 0; i < nr_range; i++) {
		/* sub-arch specific */
		ret = kernel_physical_mapping_init(mr[i].start,
						   mr[i].end,
						   mr[i].page_size_mask);
	}

	/* Update max_pfn_mapped */
	add_pfn_range_mapped(start >> PAGE_SHIFT,  ret >> PAGE_SHIFT);

	return ret >> PAGE_SHIFT;
}

/*
 * Read this:
 *
 * We need to iterate through the E820 memory map and create direct mappings
 * for only E820_RAM and E820_KERN_RESERVED regions. We cannot simply
 * create direct mappings for all pfns from [0 to max_low_pfn) and
 * [4GB to max_pfn) because of possible memory holes in high addresses
 * that cannot be marked as UC by fixed/variable range MTRRs.
 * Depending on the alignment of E820 ranges, this may possibly result
 * in using smaller size (i.e. 4K instead of 2M or 1G) page tables.
 *
 * init_mem_mapping() calls init_range_memory_mapping() with big range.
 * That range would have hole in the middle or ends, and only ram parts
 * will be mapped in init_range_memory_mapping().
 */
static unsigned long __init init_range_memory_mapping(
					   unsigned long r_start,
					   unsigned long r_end)
{
	unsigned long start_pfn, end_pfn;
	unsigned long mapped_ram_size = 0;
	int i;

	for_each_mem_pfn_range(i, MAX_NR_NODES, &start_pfn, &end_pfn, NULL) {
		u64 start = clamp_val(PFN_PHYS(start_pfn), r_start, r_end);
		u64 end = clamp_val(PFN_PHYS(end_pfn), r_start, r_end);
		if (start >= end)
			continue;

		/*
		 * if it is overlapping with brk pgt, we need to
		 * alloc pgt buf from memblock instead.
		 */
		can_use_brk_pgt = max(start, (u64)pgt_buf_end<<PAGE_SHIFT)
				 >= min(end, (u64)pgt_buf_top<<PAGE_SHIFT);

		init_memory_mapping(start, end);
		mapped_ram_size += end - start;

		can_use_brk_pgt = true;
	}

	return mapped_ram_size;
}

static unsigned long __init get_new_step_size(unsigned long step_size)
{
	/*
	 * Initial mapped size is PMD_SIZE (2M).
	 * We can not set step_size to be PUD_SIZE (1G) yet.
	 * In worse case, when we cross the 1G boundary, and
	 * PG_LEVEL_2M is not set, we will need 1+1+512 pages (2M + 8k)
	 * to map 1G range with PTE. Hence we use one less than the
	 * difference of page table level shifts.
	 *
	 * Don't need to worry about overflow in the top-down case, on 32bit,
	 * when step_size is 0, round_down() returns 0 for start, and that
	 * turns it into 0x100000000ULL.
	 * In the bottom-up case, round_up(x, 0) returns 0 though too, which
	 * needs to be taken into consideration by the code below.
	 */
	return step_size << (PMD_SHIFT - PAGE_SHIFT - 1);
}

/**
 * memory_map_top_down - Map [map_start, map_end) top down
 * @map_start: start address of the target memory range
 * @map_end: end address of the target memory range
 *
 * This function will setup direct mapping for memory range
 * [map_start, map_end) in top-down. That said, the page tables
 * will be allocated at the end of the memory, and we map the
 * memory in top-down.
 */
static void __init memory_map_top_down(unsigned long map_start,
				       unsigned long map_end)
{
	unsigned long real_end, start, last_start;
	unsigned long step_size;
	unsigned long addr;
	unsigned long mapped_ram_size = 0;

	/* xen has big range in reserved near end of ram, skip it at first.*/
	addr = memblock_find_in_range(map_start, map_end, PMD_SIZE, PMD_SIZE);
	real_end = addr + PMD_SIZE;

	/* step_size need to be small so pgt_buf from BRK could cover it */
	step_size = PMD_SIZE;
	max_pfn_mapped = 0; /* will get exact value next */
	min_pfn_mapped = real_end >> PAGE_SHIFT;
	last_start = start = real_end;

	/*
	 * We start from the top (end of memory) and go to the bottom.
	 * The memblock_find_in_range() gets us a block of RAM from the
	 * end of RAM in [min_pfn_mapped, max_pfn_mapped) used as new pages
	 * for page table.
	 */
	while (last_start > map_start) {
		if (last_start > step_size) {
			start = round_down(last_start - 1, step_size);
			if (start < map_start)
				start = map_start;
		} else
			start = map_start;
		mapped_ram_size += init_range_memory_mapping(start,
							last_start);
		last_start = start;
		min_pfn_mapped = last_start >> PAGE_SHIFT;
		if (mapped_ram_size >= step_size)
			step_size = get_new_step_size(step_size);
	}

	if (real_end < map_end)
		init_range_memory_mapping(real_end, map_end);
}

/**
 * memory_map_bottom_up - Map [map_start, map_end) bottom up
 * @map_start: start address of the target memory range
 * @map_end: end address of the target memory range
 *
 * This function will setup direct mapping for memory range
 * [map_start, map_end) in bottom-up. Since we have limited the
 * bottom-up allocation above the kernel, the page tables will
 * be allocated just above the kernel and we map the memory
 * in [map_start, map_end) in bottom-up.
 */
static void __init memory_map_bottom_up(unsigned long map_start,
					unsigned long map_end)
{
	unsigned long next, start;
	unsigned long mapped_ram_size = 0;
	/* step_size need to be small so pgt_buf from BRK could cover it */
	unsigned long step_size = PMD_SIZE;

	start = map_start;
	min_pfn_mapped = start >> PAGE_SHIFT;

	/*
	 * We start from the bottom (@map_start) and go to the top (@map_end).
	 * The memblock_find_in_range() gets us a block of RAM from the
	 * end of RAM in [min_pfn_mapped, max_pfn_mapped) used as new pages
	 * for page table.
	 */
	while (start < map_end) {
		if (step_size && map_end - start > step_size) {
			next = round_up(start + 1, step_size);
			if (next > map_end)
				next = map_end;
		} else {
			next = map_end;
		}

		mapped_ram_size += init_range_memory_mapping(start, next);
		start = next;

		if (mapped_ram_size >= step_size)
			step_size = get_new_step_size(step_size);
	}
}

/* Set identity mapping mostly */
void __init init_mem_mapping(void)
{
	unsigned long end;

	probe_page_size_mask();

#ifdef CONFIG_X86_32
	end = max_low_pfn << PAGE_SHIFT;
#else
	end = max_pfn << PAGE_SHIFT;
#endif

	/* The ISA range is always mapped regardless of memory holes */
	init_memory_mapping(0, ISA_END_ADDRESS);

	/*
	 * This looks much more complicated than it should be. All those
	 * checkings following are meant to check if it falls into usable
	 * DRAM reported by E820 map.
	 */
	if (memblock_bottom_up()) {
		unsigned long kernel_end = __pa(__kend);

		/*
		 * We need two separate calls here. This is because we want to
		 * allocate page tables above the kernel. So we first map
		 * [kernel_end, end) to make memory above the kernel be mapped
		 * as soon as possible. And then use page tables allocated above
		 * the kernel to map [ISA_END_ADDRESS, kernel_end).
		 */
		memory_map_bottom_up(kernel_end, end);
		memory_map_bottom_up(ISA_END_ADDRESS, kernel_end);
	} else {
		memory_map_top_down(ISA_END_ADDRESS, end);
	}

#ifdef CONFIG_X86_64
	if (max_pfn > max_low_pfn) {
		/* can we preseve max_low_pfn ?*/
		max_low_pfn = max_pfn;
	}
#else
	early_ioremap_page_table_range_init();
#endif

	load_cr3(initial_page_table);
	__flush_tlb_all();
}

void __init zone_init(void)
{
	unsigned long zone_max_pfns[__MAX_NR_ZONES];

	memset(zone_max_pfns, 0, sizeof(zone_max_pfns));

#ifdef CONFIG_ZONE_DMA
	zone_max_pfns[ZONE_DMA]		= min(MAX_DMA_PFN, max_low_pfn);
#endif

#ifdef CONFIG_ZONE_DMA32
	zone_max_pfns[ZONE_DMA32]	= min(MAX_DMA32_PFN, max_low_pfn);
#endif

	zone_max_pfns[ZONE_NORMAL]	= max_low_pfn;

#ifdef CONFIG_HIGHMEM
	zone_max_pfns[ZONE_HIGHMEM]	= max_pfn;
#endif
	pr_info("max_pfn: %lu, %#lx\n", max_pfn, max_pfn<<PAGE_SHIFT);

	free_area_init_nodes(zone_max_pfns);
}
