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
#include <asm/tlbflush.h>

#include <sandix/mm.h>
#include <sandix/types.h>
#include <sandix/kernel.h>
#include <sandix/export.h>
#include <sandix/bootmem.h>

#include "mm_internal.h"

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
	if (max_pfn <= MAXMEM_PFN) {
		/*
		 * All of RAM fits into low memory
		 */
		max_low_pfn = max_pfn;
	} else {
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

/*
 * Creates a middle page table and puts a pointer to it in the given global
 * directory entry. This only returns the gdt entry in non-PAE compilation
 * mode, since the middle layer is folded:
 */
static pmd_t * __init one_md_table_init(pgd_t *pgd)
{
	pud_t *pud;
	pmd_t *pmd_table;

#ifdef CONFIG_X86_PAE
	if (!(pgd_val(*pgd) & __PAGE_PRESENT)) {
		pmd_table = (pmd *)alloc_low_page();
		set_pgd(pgd, __pgd(__pa(pmd_table) | __PAGE_PRESENT));
		pud = pud_offset(pgd, 0);
		BUG_ON(pmd_table != pmd_offset(pud, 0));
	}
#else
	pud = pud_offset(pgd, 0);
	pmd_table = pmd_offset(pud, 0);
#endif

	return pmd_table;
}

/*
 * Create a page table and puts a pointer to it in the given middle page
 * directory entry:
 */
static pte_t * __init one_page_table_init(pmd_t *pmd)
{
	if (!(pmd_val(*pmd) & __PAGE_PRESENT)) {
		pte_t *page_table = (pte_t *)alloc_low_page();
		set_pmd(pmd, __pmd(__pa(page_table) | __PAGE_TABLE));
		BUG_ON(page_table != pte_offset_kernel(pmd, 0));
	}
	return pte_offset_kernel(pmd, 0);
}

static inline int is_kernel_text(unsigned long addr)
{
	if (addr >= (unsigned long)__text_start &&
	    addr <= (unsigned long)__init_end)
		return 1;
	return 0;
}

/*
 * This maps the physical memory to kernel virtual address space, a total of
 * max_low_pfn pages, by creating page tables starting from address PAGE_OFFSET:
 */
unsigned long __init kernel_physical_mapping_init(unsigned long start,
						  unsigned long end,
						  unsigned long page_size_mask)
{
	unsigned long last_map_addr = end;
	unsigned long start_pfn, end_pfn, pfn;
	int use_pse, mapping_iter;
	pgd_t *pgd_base = initial_page_table;
	pgd_t *pgd;
	pmd_t *pmd;
	pte_t *pte;
	int pgd_idx, pmd_idx, pte_idx;
	unsigned int pages_2m, pages_4k;

	use_pse = page_size_mask == (1 << PG_LEVEL_2M);
	if (!cpu_has_pse)
		use_pse = 0;

	start_pfn = start >> PAGE_SHIFT;
	end_pfn = start >> PAGE_SHIFT;

	/*
	 * First iteration will setup identity mapping using large/small pages
	 * based on use_pse, with other attributes same as set by
	 * the early code in head_32.S
	 *
	 * Second iteration will setup the appropriate attributes (NX, GLOBAL..)
	 * as desired for the kernel identity mapping.
	 *
	 * This two pass mechanism conforms to the TLB app note which says:
	 *
	 *     "Software should not write to a paging-structure entry in a way
	 *      that would change, for any linear address, both the page size
	 *      and either the page frame or attributes."
	 */
	mapping_iter = 1;

repeat:
	pages_2m = pages_4k = 0;
	pfn = start_pfn;
	pgd_idx = pgd_index((pfn << PAGE_SHIFT) + PAGE_OFFSET);
	pgd = pgd_base + pgd_idx;

	for (; pgd_idx < PTRS_PER_PGD; pgd++, pgd_idx++) {
		pmd = one_md_table_init(pgd);

		if (pfn >= end_pfn)
			continue;

#ifdef CONFIG_X86_PAE
		pmd_idx = pmd_index((pfn << PAGE_SHIFT) + PAGE_OFFSET);
		pmd += pmd_idx;
#else
		pmd_idx = 0;
#endif

		for (; pmd_idx < PTRS_PER_PMD && pfn < end_pfn;
		       pmd++, pmd_idx++) {
			unsigned int addr = pfn * PAGE_SIZE + PAGE_OFFSET;

			/*
			 * Map with big pages if possible, otherwise create
			 * normal page tables:
			 */
			if (use_pse) {
			}

			pte = one_page_table_init(pmd);
			pte_idx = pte_index((pfn << PAGE_SHIFT) + PAGE_OFFSET);
			pte += pte_idx;

			for (; pte_idx < PTRS_PER_PTE && pfn < end_pfn;
			       pte++, pfn++, pte_idx++, addr += PAGE_SIZE) {
				/*
				 * First pass will use the same initial
				 * identity mapping attribute:
				 */
				pgprot_t init_prot = __pgprot(PTE_IDENT_ATTR);
				pgprot_t prot = PAGE_KERNEL;

				if (is_kernel_text(addr))
					prot = PAGE_KERNEL_EXEC;
			
				pages_4k++;
				if (mapping_iter == 1) {
					set_pte(pte, pfn_pte(pfn, init_prot));
					last_map_addr = (pfn < PAGE_SHIFT) + PAGE_OFFSET;
				} else {
					set_pte(pte, pfn_pte(pfn, prot));
				}
			}
		}
	}
	if (mapping_iter == 1) {
		/*
		 * update direct mapping page count only in the first
		 * iteration.
		 */
		update_page_count(PG_LEVEL_2M, pages_2m);
		update_page_count(PG_LEVEL_4K, pages_4k);

		/*
		 * local global flush tlb, which will flush the previous
		 * mappings present in both small and large page TLB's.
		 */
		__flush_tlb_all();

		/*
		 * Second iteration will set the actual desired PTE attributes.
		 */
		mapping_iter = 2;
		goto repeat;
	}
	return last_map_addr;
}
