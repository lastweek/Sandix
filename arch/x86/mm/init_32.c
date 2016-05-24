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

#include <asm/page.h>
#include <asm/fixmap.h>
#include <asm/pgtable.h>
#include <asm/sections.h>

#include <sandix/mm.h>
#include <sandix/types.h>
#include <sandix/kernel.h>
#include <sandix/export.h>
#include <sandix/bootmem.h>

void *high_memory;
EXPORT_SYMBOL(high_memory);

unsigned long highstart_pfn, highend_pfn;

pteval_t __supported_pte_mask __read_mostly = ~(__PAGE_NX | __PAGE_GLOBAL);
EXPORT_SYMBOL(__supported_pte_mask);

static void __init check_highmem_config(void)
{
#define MAX_NONPAE_PFN (1 << 20)
#ifndef CONFIG_HIGHMEM
	/* Maximum memory usable is what is directly addressable */
	printk(KERN_WARNING "WARNING only %ldMB will be used.\n", MAXMEM>>20);
	if (max_pfn > MAX_NONPAE_PFN)
		printk(KERN_WARNING "Use a HIGHMEM64G enabled kernel.\n");
	else
		printk(KERN_WARNING "Use a HIGHMEM enabled kernel.\n");
	max_pfn = MAXMEM_PFN;
#else /* !CONFIG_HIGHMEM */
#ifndef CONFIG_HIGHMEM64G
	if (max_pfn > MAX_NONPAE_PFN) {
		max_pfn = MAX_NONPAE_PFN;
		printk(KERN_WARNING "WARNING only 4GB will be used. Use a HIGHMEM64G enabled kernel!\n");
	}
#endif /* !CONFIG_HIGHMEM64G */
#endif /* !CONFIG_HIGHMEM */
#undef MAX_NONPAE_PFN
}

/* Determine low and high memory ranges */
void __init find_low_pfn_range(void)
{
	if (max_pfn <= MAXMEM_PFN)
		/* All of RAM fits into low memory */
		max_low_pfn = max_pfn;
	else {
		/*
		 * We have more RAM than fits into low memory,
		 * try to put it into high memory.
		 */
		max_low_pfn = MAXMEM_PFN;
		check_highmem_config();
	}

	printk(KERN_NOTICE "%ldMB LOWMEM available.\n", pages_to_mb(max_low_pfn));
#ifdef CONFIG_HIGHMEM
	highstart_pfn = highend_pfn = max_pfn;
	if (max_pfn > max_low_pfn)
		highstart_pfn = max_low_pfn;
	printk(KERN_NOTICE "%ldMB HIGHMEM avaiable.\n", pages_to_mb(highend_pfn-highstart_pfn));
	high_memory = (void *) __va(highstart_pfn * PAGE_SIZE - 1) + 1;
#else
	high_memory = (void *) __va(max_low_pfn * PAGE_SIZE - 1) + 1;
#endif
	mem_init();
}

void __init mem_init(void)
{
	high_memory = (void *)__va(max_low_pfn * PAGE_SIZE -1) + 1;

	printk(KERN_INFO
		"virtual kernel memory layout:\n"
		"    fixmap  : 0x%08lx - 0x%08lx   (%4ld kB)\n"
#ifdef CONFIG_HIGHMEM
		"    pkmap   : 0x%08lx - 0x%08lx   (%4ld kB)\n"
#endif
		"    vmalloc : 0x%08lx - 0x%08lx   (%4ld MB)\n"
		"    lowmem  : 0x%08lx - 0x%08lx   (%4ld MB)\n"
		"      .init : 0x%08lx - 0x%08lx   (%4ld kB)\n"
		"      .data : 0x%08lx - 0x%08lx   (%4ld kB)\n"
		"      .text : 0x%08lx - 0x%08lx   (%4ld kB)\n",
		FIXADDR_START, FIXADDR_TOP, (FIXADDR_TOP - FIXADDR_START) >> 10,

#ifdef CONFIG_HIGHMEM
		PKMAP_BASE, PKMAP_BASE + LAST_PKMAP * PAGE_SIZE, (LAST_PKMAP * PAGE_SIZE) >> 10,
#endif
		VMALLOC_START, VMALLOC_END, (VMALLOC_END - VMALLOC_START) >> 20,

		(unsigned long)__va(0), (unsigned long)high_memory,
		((unsigned long)high_memory - (unsigned long)__va(0)) >> 20,

		(unsigned long)&__init_start, (unsigned long)&__init_end,
		((unsigned long)&__init_end - (unsigned long)&__init_start) >> 10,

		(unsigned long)&__rodata_start, (unsigned long)&__data_end,
		((unsigned long)&__data_end - (unsigned long)&__rodata_start) >> 10,

		(unsigned long)&__text_start, (unsigned long)&__text_end,
		((unsigned long)&__text_end - (unsigned long)&__text_start) >> 10);

}
