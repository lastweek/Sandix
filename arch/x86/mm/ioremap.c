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

/*
 * Re-map IO memory to kernel address space so that we can access it.
 * This is needed for high PCI addresses that aren't mapped in the
 * 640k-1MB IO memory area on PC's
 */

#include <asm/fixmap.h>
#include <asm/pgalloc.h>
#include <asm/pgtable.h>
#include <asm/tlbflush.h>
#include <asm/cacheflush.h>
#include <asm/early_ioremap.h>

#include <sandix/sched.h>
#include <sandix/kernel.h>
#include <sandix/string.h>

/*
 * Early ioremap/iounmap related
 * Used before full paging and ioremap are functional
 */

static pte_t bm_pte[PAGE_SIZE/sizeof(pte_t)] __aligned(PAGE_SIZE);

static inline pte_t * __init early_ioremap_pte(unsigned long addr)
{
	return &bm_pte[pte_index(addr)];
}

bool __init is_early_ioremap_ptep(pte_t *ptep)
{
	return ptep >= &bm_pte[0] && ptep < &bm_pte[PAGE_SIZE/sizeof(pte_t)];
}

static pmd_t * __init early_ioremap_pmd(unsigned long addr)
{
	pgd_t *base = initial_page_table;
	pgd_t *pgd = &base[pgd_index(addr)];
	pud_t *pud = pud_offset(pgd, addr);
	pmd_t *pmd = pmd_offset(pud, addr);

	return pmd;
}

void __init __early_set_fixmap(enum fixed_addresses idx,
			       phys_addr_t phys, pgprot_t flags)
{
	unsigned long vaddr = __fix_to_virt(idx);
	pte_t *ptep;

	if (idx >= __end_of_fixed_addresses) {
		BUG();
		return;
	}

	ptep = early_ioremap_pte(vaddr);

	if (pgprot_val(flags))
		set_pte(ptep, pfn_pte(phys >> PAGE_SHIFT, flags));
	else
		pte_clear(&init_mm, vaddr, ptep);

	__flush_tlb_one(vaddr);
}

/**
 * early_ioremap_init	-	Arch-specific initialization of early_ioremap
 *
 * early_ioremap() and early_iounmap() are for temporary early
 * boot-time mappings, before the real ioremap() is functional.
 */
void __init early_ioremap_init(void)
{
	pmd_t *pmd;

#ifdef CONFIG_X86_64
	BUILD_BUG_ON((fix_to_virt(0) + PAGE_SIZE) & ((1 << PMD_SHIFT) - 1));
#else
	WARN_ON((fix_to_virt(0) + PAGE_SIZE) & ((1 << PMD_SHIFT) - 1));
#endif

	/*
	 * The boot-ioremap range spans multiple pmds, for which
	 * we are not prepared:
	 */
#define __FIXADDR_TOP (-PAGE_SIZE)
	BUILD_BUG_ON((__fix_to_virt(FIX_BTMAP_BEGIN) >> PMD_SHIFT) !=
		     (__fix_to_virt(FIX_BTMAP_END) >> PMD_SHIFT));
#undef __FIXADDR_TOP

	/* Generic initialization: */
	early_ioremap_setup();

	pmd = early_ioremap_pmd(fix_to_virt(FIX_BTMAP_BEGIN));
	memset(bm_pte, 0, sizeof(bm_pte));
	pmd_populate_kernel(&init_mm, pmd, bm_pte);

#ifdef CONFIG_PRINT_FIXMAP_RANGE
	printk(KERN_INFO "fix_to_virt(FIX_BTMAP_BEGIN): %08lx\n", fix_to_virt(FIX_BTMAP_BEGIN));
	printk(KERN_INFO "fix_to_virt(FIX_BTMAP_END):   %08lx\n", fix_to_virt(FIX_BTMAP_END));
	printk(KERN_INFO "FIX_BTMAP_END:       %d\n", FIX_BTMAP_END);
	printk(KERN_INFO "FIX_BTMAP_BEGIN:     %d\n", FIX_BTMAP_BEGIN);
#endif
}
