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

#ifndef _ASM_GENERIC_MEMORY_MODEL_H_
#define _ASM_GENERIC_MEMORY_MODEL_H_

/*
 * Handle 3 memory models:
 *  -  FLATMEM
 *  -  DOSCPNTIGMEM
 *  -  SPARSEMEM
 */

#if defined(CONFIG_FLATMEM)

#define __pfn_to_page(pfn)	(mem_map + (pfn))
#define __page_to_pfn(page)	((unsigned long)((page) - mem_map))

#elif defined(CONFIG_DISCONTIGMEM)

#define __pfn_to_page(pfn)								\
({	unsigned long __pfn = (pfn);							\
	unsigned long __nid = pfn_to_nid(__pfn);					\
	NODE_DATA(__nid)->node_mem_map + __pfn - NODE_DATA(__nid)->node_start_pfn;	\
})

#define __page_to_pfn(pg)								\
({	const struct page *__pg = (pg);							\
	struct pglist_data *__pgdat = NODE_DATA(page_to_nid(__pg));			\
	(unsigned long)(__pg - __pgdat->node_mem_map) +					\
	 __pgdat->node_start_pfn;							\
})

#elif defined(CONFIG_SPARSEMEM_VMEMMAP)

/* memmap is virtually contiguous.  */
#define __pfn_to_page(pfn)	(vmemmap + (pfn))
#define __page_to_pfn(page)	(unsigned long)((page) - vmemmap)

#elif defined(CONFIG_SPARSEMEM)
/*
 * Note: section's mem_map is encoded to reflect its start_pfn.
 * section[i].section_mem_map == mem_map's address - start_pfn;
 */
#define __page_to_pfn(pg)					\
({	const struct page *__pg = (pg);				\
	int __sec = page_to_section(__pg);			\
	(unsigned long)(__pg - __section_mem_map_addr(__nr_to_section(__sec)));	\
})

#define __pfn_to_page(pfn)				\
({	unsigned long __pfn = (pfn);			\
	struct mem_section *__sec = __pfn_to_section(__pfn);	\
	__section_mem_map_addr(__sec) + __pfn;		\
})
#endif /* CONFIG_FLATMEM/DISCONTIGMEM/SPARSEMEM */

/*
 * Convert a physical address to a Page Frame Number and back
 */
#define	__phys_to_pfn(paddr)	PHYS_PFN(paddr)
#define	__pfn_to_phys(pfn)	PFN_PHYS(pfn)

#define page_to_pfn		__page_to_pfn
#define pfn_to_page		__pfn_to_page

#endif /* _ASM_GENERIC_MEMORY_MODEL_H_ */
