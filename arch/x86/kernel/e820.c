/*
 *	Copyright (C) 2015-2016 Yizhou Shan <shanyizhou@ict.ac.cn>
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

#include <sandix/page.h>
#include <sandix/types.h>
#include <sandix/string.h>
#include <sandix/printk.h>

#include <asm/e820.h>
#include <asm/setup.h>
#include <asm/bootparam.h>

struct e820map e820;

void __init setup_memory_map(void)
{
	int i, nr;
	struct e820entry *entry, *dst, *src;
	
	dst = e820.map;
	src = boot_params.e820_map;
	memcpy((void *)dst, (void *)src, sizeof(struct e820entry) * E820MAX);
	nr = e820.nr_entries = boot_params.e820_nr_entries;

	for (i = 0; i < nr; i++) {
		printk("Range: %15llX ~ %15llX \t %d\n", e820.map[i].addr,
			e820.map[i].addr + e820.map[i].size, e820.map[i].type);
	}
}
