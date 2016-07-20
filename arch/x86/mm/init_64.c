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
#include <sandix/mm_zone.h>
#include <sandix/types.h>
#include <sandix/kernel.h>
#include <sandix/export.h>
#include <sandix/bootmem.h>

/*
 * This maps the physical memory to kernel virtual address space, a total of
 * max_low_pfn pages, by creating page tables starting from address PAGE_OFFSET:
 */
unsigned long __init kernel_physical_mapping_init(unsigned long start,
						  unsigned long end,
						  unsigned long page_size_mask)
{
	bool pgd_changed = false;
	unsigned long next, last_map_addr = end;
	unsigned long addr;

	start = (unsigned long)__va(start);
	end = (unsigned long)__va(end);
	addr = start;

	for (; start < end; start = next) {
		pgd_t *pgd = pgd_offset_k(start);
		pud_t *pud;

		next = (start & PGDIR_MASK) + PGDIR_SIZE;

		if (pgd_val(*pgd)) {
			pud = (pud_t *)pgd_page_vaddr(*pgd);
			last_map_addr = phys_pud_init(pud, __pa(start),
						 __pa(end), page_size_mask);
			continue;
		}

		pud = alloc_low_page();
		last_map_addr = phys_pud_init(pud, __pa(start), __pa(end),
						 page_size_mask);

		spin_lock(&init_mm.page_table_lock);
		pgd_populate(&init_mm, pgd, pud);
		spin_unlock(&init_mm.page_table_lock);
		pgd_changed = true;
	}

	if (pgd_changed)
		sync_global_pgds(addr, end - 1, 0);

	__flush_tlb_all();

	return last_map_addr;
}

#ifndef CONFIG_NUMA
void __init init_mem_init(void)
{

}
#endif

void __init paging_init(void)
{
	sparse_memory_present_with_active_regions(MAX_NR_NODES);
	zone_init();
}

void __init native_pagetable_init(void)
{
	paging_init();
}
