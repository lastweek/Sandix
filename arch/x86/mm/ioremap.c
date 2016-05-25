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

#include <asm/fixmap.h>
#include <asm/pgalloc.h>
#include <asm/pgtable.h>

#include <sandix/sched.h>
#include <sandix/kernel.h>
#include <sandix/string.h>

static pte_t bm_pte[PAGE_SIZE/sizeof(pte_t)] __aligned(PAGE_SIZE);

static void *prev_map[FIX_BTMAPS_SLOTS] __initdata;
static unsigned long prev_siz[FIX_BTMAPS_SLOTS] __initdata;
static unsigned long slot_virt[FIX_BTMAPS_SLOTS] __initdata;

static pmd_t * __init early_ioremap_pmd(unsigned long addr)
{
	pgd_t *base = initial_page_table;
	pgd_t *pgd = &base[pgd_index(addr)];
	pud_t *pud = pud_offset(pgd, addr);
	pmd_t *pmd = pmd_offset(pud, addr);

	return pmd;
}

static void __init early_ioremap_setup(void)
{
	int i;

	for (i = 0; i < FIX_BTMAPS_SLOTS; i++)
		slot_virt[i] = __fix_to_virt(FIX_BTMAP_BEGIN - i * NR_FIX_BTMAPS);
}

/*
 * early_ioremap() and early_iounmap() are for temporary early
 * boot-time mappings, before the real ioremap() is functional.
 */
void __init early_ioremap_init(void)
{
	pmd_t *pmd;

	early_ioremap_setup();

	pmd = early_ioremap_pmd(fix_to_virt(FIX_BTMAP_BEGIN));
	memset(bm_pte, 0, sizeof(bm_pte));
	pmd_populate_kernel(&init_mm, pmd, bm_pte);

#ifdef CONFIG_PRINT_FIXMAP_RANG
	printk(KERN_INFO "fix_to_virt(FIX_BTMAP_BEGIN): %08lx\n", fix_to_virt(FIX_BTMAP_BEGIN));
	printk(KERN_INFO "fix_to_virt(FIX_BTMAP_END):   %08lx\n", fix_to_virt(FIX_BTMAP_END));
	printk(KERN_INFO "FIX_BTMAP_END:       %d\n", FIX_BTMAP_END);
	printk(KERN_INFO "FIX_BTMAP_BEGIN:     %d\n", FIX_BTMAP_BEGIN);
#endif
}
