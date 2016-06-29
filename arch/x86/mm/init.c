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

#include <asm/e820.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/sections.h>

#include <sandix/mm.h>
#include <sandix/pfn.h>
#include <sandix/export.h>
#include <sandix/kernel.h>
#include <sandix/printk.h>
#include <sandix/kconfig.h>

#include "mm_internal.h"

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

static unsigned int page_size_mask;

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

	pr_debug("init_mmeory_mapping: [mem %#010lx-%#010lx]\n",
		start, end - 1);

	memset(mr, 0, sizeof(mr));
	nr_range = split_mem_range(mr, 0, start, end);

	for (i = 0; i < nr_range; i++) {
		/* sub-arch specific */
		ret = kernel_physical_mapping_init(mr[i].start,
						   mr[i].end,
						   mr[i].page_size_mask);
	}
	return 0;
}

void __init init_mem_mapping(void)
{
	unsigned long end;
	unsigned long kernel_end = __pa(__kend);

	if (cpu_has_pse)
		pr_info("PSE is enabled\n");
	else
		pr_info("PSE is disabled\n");

#ifdef CONFIG_X86_32
	end = max_low_pfn << PAGE_SHIFT;
#else
	end = max_pfn << PAGE_SHIFT;
#endif

	/* the ISA range is always mapped regardless of memory holes */
	init_memory_mapping(0, ISA_END_ADDRESS);
}
