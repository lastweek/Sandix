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
#include <sandix/compiler.h>

#include <asm/e820.h>
#include <asm/setup.h>
#include <asm/bootparam.h>

struct e820map e820;

static const char *e820_type_to_string(unsigned int e820_type)
{
	switch (e820_type) {
	case E820_RESERVED_KERN:
	case E820_RAM:		return "System RAM";
	case E820_ACPI:		return "ACPI Tables";
	case E820_NVS:		return "ACPI Non-volatile Storage";
	case E820_UNUSABLE:	return "Unusable memory";
	case E820_PRAM:		return "Persistent Memory (legacy)";
	case E820_PMEM:		return "Persistent Memory";
	default:		return "Reserved";
	}
}

void __init e820_print_map(char *who)
{
	int i;

	for (i = 0; i < e820.nr_entries; i++) {
		printk(KERN_INFO "%s: [mem %#018Lx-%#018Lx] %s\n", who,
			e820.map[i].addr, (e820.map[i].addr + e820.map[i].size - 1),
			e820_type_to_string(e820.map[i].type));
	}
}

void __init setup_memory_map(void)
{
	memcpy((void *)e820.map, (void *)boot_params.e820_map, sizeof(struct e820entry) * E820MAX);
	e820.nr_entries = boot_params.e820_nr_entries;

	printk(KERN_INFO "e820: BIOS-provided physical RAM map:\n");
	e820_print_map("BIOS-e820");
}
