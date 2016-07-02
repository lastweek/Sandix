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
#include <asm/early_ioremap.h>

#include <sandix/mm.h>
#include <sandix/types.h>
#include <sandix/kernel.h>
#include <sandix/export.h>
#include <sandix/bootmem.h>
#include <sandix/memblock.h>

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
}

void __init setup_bootmem_allocator(void)
{
	pr_info("  mapped low ram: 0 - %#lx\n", max_pfn_mapped<<PAGE_SHIFT);
	pr_info("  low ram: 0 - %#lx\n", max_low_pfn<<PAGE_SHIFT);
}

/*
 * init_mem initialization without NUMA
 */
#ifndef CONFIG_NUMA
void __init init_mem_init(void)
{
#ifdef CONFIG_HIGHMEM
	highstart_pfn = highend_pfn = max_pfn;
	if (max_pfn > max_low_pfn)
		highstart_pfn = max_low_pfn;
	pr_info("%ldMB HIGHMEM avaiable.\n",
		pages_to_mb(highend_pfn-highstart_pfn));
	high_memory = (void *) __va(highstart_pfn * PAGE_SIZE - 1) + 1;
#else
	high_memory = (void *) __va(max_low_pfn * PAGE_SIZE - 1) + 1;
#endif
	pr_info("%ldMB LOWMEM available.\n",
		pages_to_mb(max_low_pfn));

	memblock_set_node(0, (phys_addr_t)ULLONG_MAX, &memblock.memory, 0);

	setup_bootmem_allocator();

	/* XXX: RemoveMe */
	mem_init();
}
#endif

void __init mem_init(void)
{
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
	unsigned long last_map_paddr = end;
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
	end_pfn = end >> PAGE_SHIFT;

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
				unsigned int addr2;
				/*
				 * First pass will use the same initial
				 * identity mapping attribute + _PAGE_PSE:
				 */
				pgprot_t init_prot = __pgprot(PTE_IDENT_ATTR | __PAGE_PSE);
				pgprot_t prot = PAGE_KERNEL_LARGE;

				pfn &= PMD_MASK >> PAGE_SHIFT;
				addr2 = (pfn + PTRS_PER_PTE-1) * PAGE_SIZE +
					PAGE_OFFSET + PAGE_SIZE-1;

				if (is_kernel_text(addr) ||
				    is_kernel_text(addr2))
					prot = PAGE_KERNEL_LARGE_EXEC;

				pages_2m++;
				if (mapping_iter == 1)
					set_pmd(pmd, pfn_pmd(pfn, init_prot));
				else
					set_pmd(pmd, pfn_pmd(pfn, prot));

				pfn += PTRS_PER_PTE;
				continue;
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
					last_map_paddr = (pfn << PAGE_SHIFT) + PAGE_SIZE;
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
	return last_map_paddr;
}

static unsigned long __init page_table_range_init_count(unsigned long start,
							unsigned long end)
{
	unsigned long count = 0;
#ifdef CONFIG_HIGHMEM
	int pmd_idx_kmap_begin = fix_to_virt(FIX_KMAP_END) >> PMD_SHIFT;
	int pmd_idx_kmap_end = fix_to_virt(FIX_KMAP_BEGIN) >> PMD_SHIFT;
	int pgd_idx, pmd_idx;
	unsigned long vaddr;

	if (pmd_idx_kmap_begin == pmd_idx_kmap_end)
		return 0;

	vaddr = start;
	pgd_idx = pgd_index(vaddr);
	pmd_idx = pmd_index(vaddr);

	for ( ; (pgd_idx < PTRS_PER_PGD) && (vaddr != end); pgd_idx++) {
		for (; (pmd_idx < PTRS_PER_PMD) && (vaddr != end);
							pmd_idx++) {
			if ((vaddr >> PMD_SHIFT) >= pmd_idx_kmap_begin &&
			    (vaddr >> PMD_SHIFT) <= pmd_idx_kmap_end)
				count++;
			vaddr += PMD_SIZE;
		}
		pmd_idx = 0;
	}
#endif
	return count;
}

static pte_t *__init page_table_kmap_check(pte_t *pte, pmd_t *pmd,
					   unsigned long vaddr, pte_t *lastpte,
					   void **adr)
{
#ifdef CONFIG_HIGHMEM
	/*
	 * Something (early fixmap) may already have put a pte
	 * page here, which causes the page table allocation
	 * to become nonlinear. Attempt to fix it, and if it
	 * is still nonlinear then we have to bug.
	 */
	int pmd_idx_kmap_begin = fix_to_virt(FIX_KMAP_END) >> PMD_SHIFT;
	int pmd_idx_kmap_end = fix_to_virt(FIX_KMAP_BEGIN) >> PMD_SHIFT;

	if (pmd_idx_kmap_begin != pmd_idx_kmap_end
	    && (vaddr >> PMD_SHIFT) >= pmd_idx_kmap_begin
	    && (vaddr >> PMD_SHIFT) <= pmd_idx_kmap_end) {
		pte_t *newpte;
		int i;

		newpte = *adr;
		for (i = 0; i < PTRS_PER_PTE; i++)
			set_pte(newpte + i, pte[i]);
		*adr = (void *)(((unsigned long)(*adr)) + PAGE_SIZE);

		set_pmd(pmd, __pmd(__pa(newpte)|__PAGE_TABLE));
		BUG_ON(newpte != pte_offset_kernel(pmd, 0));
		__flush_tlb_all();

		pte = newpte;
	}
	BUG_ON(vaddr < fix_to_virt(FIX_KMAP_BEGIN - 1)
	       && vaddr > fix_to_virt(FIX_KMAP_END)
	       && lastpte && lastpte + PTRS_PER_PTE != pte);
#endif
	return pte;
}

/*
 * This function initializes a certain range of kernel virtual memory
 * with new bootmem page tables, everywhere page tables are missing in
 * the given range.
 *
 * NOTE: The pagetables are allocated contiguous on the physical space
 * so we can cache the place of the first one and move around without
 * checking the pgd every time.
 */
static void __init page_table_range_init(unsigned long start, unsigned long end,
					 pgd_t *pgd_base)
{
	int pgd_idx, pmd_idx;
	unsigned long vaddr;
	pgd_t *pgd;
	pmd_t *pmd;
	pte_t *pte = NULL;
	unsigned long count;
	void *adr = NULL;

	count = page_table_range_init_count(start, end);
	if (count)
		adr = alloc_low_pages(count);

	vaddr = start;
	pgd_idx = pgd_index(vaddr);
	pmd_idx = pmd_index(vaddr);
	pgd = pgd_base + pgd_idx;

	for ( ; (pgd_idx < PTRS_PER_PGD) && (vaddr != end); pgd++, pgd_idx++) {
		pmd = one_md_table_init(pgd);
		pmd = pmd + pmd_index(vaddr);
		for (; (pmd_idx < PTRS_PER_PMD) && (vaddr != end);
							pmd++, pmd_idx++) {
			pte = page_table_kmap_check(one_page_table_init(pmd),
						    pmd, vaddr, pte, &adr);

			vaddr += PMD_SIZE;
		}
		pmd_idx = 0;
	}
}

/*
 * Build a proper pagetable for the kernel mappings.  Up until this
 * point, we've been running on some set of pagetables constructed by
 * the boot process.
 *
 * If we're booting on native hardware, this will be a pagetable
 * constructed in arch/x86/kernel/head_32.S.  The root of the
 * pagetable will be swapper_pg_dir.
 *
 * If we're booting paravirtualized under a hypervisor, then there are
 * more options: we may already be running PAE, and the pagetable may
 * or may not be based in swapper_pg_dir.  In any case,
 * paravirt_pagetable_init() will set up swapper_pg_dir
 * appropriately for the rest of the initialization to work.
 *
 * In general, pagetable_init() assumes that the pagetable may already
 * be partially populated, and so it avoids stomping on any existing
 * mappings.
 */
void __init early_ioremap_page_table_range_init(void)
{
	pgd_t *pgd_base = initial_page_table;
	unsigned long vaddr, end;

	/*
	 * Fixed mappings, only the page table structure has to be
	 * created - mappings will be set by set_fixmap():
	 */
	vaddr = __fix_to_virt(__end_of_fixed_addresses - 1) & PMD_MASK;
	end = (FIXADDR_TOP + PMD_SIZE - 1) & PMD_MASK;
	page_table_range_init(vaddr, end, pgd_base);
	early_ioremap_reset();
}

pte_t *pkmap_pte;

#ifdef CONFIG_HIGHMEM
static void __init permanent_kmap_init(void)
{
	unsigned long vaddr;
	pgd_t *pgd, *base = initial_page_table;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;

	vaddr = PKMAP_BASE;
	page_table_range_init(vaddr, vaddr + PAGE_SIZE * LAST_PKMAP, base);

	pgd = base + pgd_index(vaddr);
	pud = pud_offset(pgd, vaddr);
	pmd = pmd_offset(pud, vaddr);
	pte = pte_offset_kernel(pmd, vaddr);

	/* Save PKMAP PTE for HIGHMEM handling */
	pkmap_pte = pte;
}
#else
static inline void permanent_kmap_init(void) { }
#endif

pte_t *kmap_pte;

static void __init kmap_init(void)
{
	unsigned long kmap_vstart;
	pgd_t *pgd, *base = initial_page_table;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;

	kmap_vstart = fix_to_virt(FIX_KMAP_BEGIN);
	pgd = base + pgd_index(kmap_vstart);
	pud = pud_offset(pgd, kmap_vstart);
	pmd = pmd_offset(pud, kmap_vstart);
	pte = pte_offset_kernel(pmd, kmap_vstart);

	/* Save KMAP PTE */
	kmap_pte = pte;
}

/*
 * This function sets up the page tables. Note that the lowmem identidy
 * mapping is already mapped by head.S and init_mem_mapping().
 *
 * This function also unmaps the page at virtual kernel address 0, so that we
 * can trap those pesky NULL-reference errors in the kernel.
 */
void __init paging_init(void)
{
	permanent_kmap_init();
	__flush_tlb_all();
	kmap_init();

	/*
	 * Note that: at this point the bootmem allocator is fully available
	 */

	
}

void __init native_pagetable_init(void)
{
	unsigned long va, pfn;
	pgd_t *pgd, *base = initial_page_table;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;

	/*
	 * Remove any mappings which extend past the end of physical memory from
	 * the boot time page table.
	 *
	 * In virtual address space, we should have at least two pages from
	 * VMALLOC_END to pkmap or fixmap according to VMALLOC_END definition. And
	 * max_low_pfn is set to VMALLOC_END physical address. If initial memory
	 * mapping is doing right job, we should have pte used near max_low_pfn or one
	 * pmd is not present.
	 */
	for (pfn = max_low_pfn; pfn < 1<<(32-PAGE_SHIFT); pfn++) {
		va = PAGE_OFFSET + (pfn<<PAGE_SHIFT);
		pgd = base + pgd_index(va);
		if (!pgd_present(*pgd))
			break;

		pud = pud_offset(pgd, va);
		pmd = pmd_offset(pud, va);
		if (!pmd_present(*pmd))
			break;

		/* should not be large page here */
		if (pmd_large(*pmd)) {
			pr_warn("Try to clear pte for RAM above max_low_pfn: "
				"pfn: %lu pmd: %p pmd phys: %lx, "
				"BUT pmd is big page and is not using pte !\n",
				pfn, pmd, __pa(pmd));
			BUG_ON(1);
		}

		pte = pte_offset_kernel(pmd, va);
		if (!pte_present(*pte))
			break;

		pr_debug("Clearing pte for RAM above max_low_pfn: "
			 "pfn: %lx pmd: %p pmd phys: %lx pte: %p pte phys: %lx\n",
			pfn, pmd, __pa(pmd), pte, __pa(pte));
		pte_clear(NULL, va, pte);
	}

	paging_init();
}
