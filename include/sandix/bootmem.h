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

#ifndef _SANDIX_BOOTMEM_H_
#define _SANDIX_BOOTMEM_H_

#include <sandix/cache.h>
#include <sandix/mm_zone.h>

extern unsigned long max_low_pfn;
extern unsigned long min_low_pfn;

/*
 * highest page
 */
extern unsigned long max_pfn;
/*
 * highest possible page
 */
extern unsigned long long max_possible_pfn;

void free_bootmem_node(pg_data_t *pgdat, unsigned long physaddr, unsigned long size);
void free_bootmem(unsigned long physaddr, unsigned long size);

void *__alloc_bootmem(unsigned long size, unsigned long align, unsigned long goal);
void *__alloc_bootmem_nopanic(unsigned long size, unsigned long align, unsigned long goal);
void *__alloc_bootmem_node(pg_data_t *pgdat, unsigned long size, unsigned long align, unsigned long goal);
void *__alloc_bootmem_node_high(pg_data_t *pgdat, unsigned long size, unsigned long align, unsigned long goal);
void *__alloc_bootmem_node_nopanic(pg_data_t *pgdat, unsigned long size, unsigned long align, unsigned long goal);
void *___alloc_bootmem_node_nopanic(pg_data_t *pgdat, unsigned long size, unsigned long align, unsigned long goal, unsigned long limit);

/* We are using top down, so it is safe to use 0 here */
#define BOOTMEM_LOW_LIMIT 0

#define alloc_bootmem(x) \
	__alloc_bootmem(x, L1_CACHE_BYTES, BOOTMEM_LOW_LIMIT)
#define alloc_bootmem_align(x, align) \
	__alloc_bootmem(x, align, BOOTMEM_LOW_LIMIT)
#define alloc_bootmem_nopanic(x) \
	__alloc_bootmem_nopanic(x, L1_CACHE_BYTES, BOOTMEM_LOW_LIMIT)
#define alloc_bootmem_pages(x) \
	__alloc_bootmem(x, PAGE_SIZE, BOOTMEM_LOW_LIMIT)
#define alloc_bootmem_pages_nopanic(x) \
	__alloc_bootmem_nopanic(x, PAGE_SIZE, BOOTMEM_LOW_LIMIT)
#define alloc_bootmem_node(pgdat, x) \
	__alloc_bootmem_node(pgdat, x, L1_CACHE_BYTES, BOOTMEM_LOW_LIMIT)
#define alloc_bootmem_node_nopanic(pgdat, x) \
	__alloc_bootmem_node_nopanic(pgdat, x, L1_CACHE_BYTES, BOOTMEM_LOW_LIMIT)
#define alloc_bootmem_pages_node(pgdat, x) \
	__alloc_bootmem_node(pgdat, x, PAGE_SIZE, BOOTMEM_LOW_LIMIT)
#define alloc_bootmem_pages_node_nopanic(pgdat, x) \
	__alloc_bootmem_node_nopanic(pgdat, x, PAGE_SIZE, BOOTMEM_LOW_LIMIT)

#endif /* _SANDIX_BOOTMEM_H_ */
