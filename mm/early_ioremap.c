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
 * Provide common bits of early_ioremap() support for architectures needing
 * temporary mappings during boot before ioremap() is available.
 */

#include <asm/fixmap.h>
#include <asm/pgalloc.h>
#include <asm/pgtable.h>

#include <sandix/bug.h>
#include <sandix/kernel.h>
#include <sandix/string.h>

void __init __weak early_ioremap_shutdown(void)
{
}

static void __iomem *prev_map[FIX_BTMAPS_SLOTS] __initdata;
static unsigned long prev_size[FIX_BTMAPS_SLOTS] __initdata;
static unsigned long slot_virt[FIX_BTMAPS_SLOTS] __initdata;

void __init early_ioremap_setup(void)
{
	int i;

	for (i = 0; i < FIX_BTMAPS_SLOTS; i++)
		if (WARN_ON_ONCE(prev_map[i]))
			break;

	for (i = 0; i < FIX_BTMAPS_SLOTS; i++)
		slot_virt[i] = __fix_to_virt(FIX_BTMAP_BEGIN - i * NR_FIX_BTMAPS);
}

void __init early_iounmap(void __iomem *addr, unsigned long size)
{
}

/* Remap an IO device */
void __init __iomem *
early_ioremap(resource_size_t phys_addr, unsigned long size)
{
}

/* Remap memory */
void __init *
early_memremap(resource_size_t phys_addr, unsigned long size)
{
}
