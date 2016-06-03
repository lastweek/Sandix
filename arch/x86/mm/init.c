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
#include <sandix/export.h>
#include <sandix/kernel.h>

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

unsigned long __init init_memory_mapping(unsigned long start,
					 unsigned long end)
{
	printk(KERN_DEBUG "init_mmeory_mapping: [mem %#010lx-%#010lx]\n",
		start, end - 1);
	return 0;
}

void __init init_mem_mapping(void)
{
	unsigned long end;
	unsigned long kernel_end = __pa(__kend);

#ifdef CONFIG_X86_32
	end = max_low_pfn << PAGE_SHIFT;
#else
	end = max_pfn << PAGE_SHIFT;
#endif

	/* the ISA range is always mapped regardless of memory holes */
	init_memory_mapping(0, ISA_END_ADDRESS);


}
