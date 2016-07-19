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

/*
 * Sparse Memory Mapping Handling
 */

#include <sandix/mm.h>
#include <sandix/mm_zone.h>
#include <sandix/bug.h>
#include <sandix/export.h>

/**
 * sparsemem_section
 * Permanent SPARSEMEM memory sections:
 */
#ifdef CONFIG_SPARSEMEM_EXTREME
struct sparsemem_section *sparsemem_section[NR_SECTION_ROOTS];
#else
struct sparsemem_section sparsemem_section[NR_SECTION_ROOTS][SECTIONS_PER_ROOT];
#endif
EXPORT_SYMBOL(sparsemem_section);

/**
 * sparse_init()	-	Init SPARSEMEM mgmt subsystem
 *
 * Allocate the accumulated non-linear sections, allocate a mem_map
 * for each and record the physical to section mapping.
 */
void __init sparse_init(void)
{
	BUG();
}
