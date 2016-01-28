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

#include <sandix/types.h>
#include <sandix/export.h>
#include <sandix/compiler.h>
#include <sandix/bootmem.h>
#include <sandix/screen_info.h>

#include <asm/e820.h>
#include <asm/segment.h>
#include <asm/sections.h>
#include <asm/bootparam.h>
#include <asm/descriptor.h>
#include <asm/pci_early.h>

unsigned long max_pfn_mapped;

unsigned long brk_start = (unsigned long)__brk_start;
unsigned long brk_end = (unsigned long)__brk_start;

struct boot_params boot_params;
EXPORT_SYMBOL(boot_params);

/* Used by low-level console drivers */
struct screen_info screen_info;
EXPORT_SYMBOL(screen_info);

struct desc_struct gdt_table[GDT_ENTRIES] __aligned(8) =
{
	/* Present, DPL=0, Execute/Read */
	/* Present, DPL=0, Read/Write */
	/* Present, DPL=3, Execute/Read */
	/* Present, DPL=3, Read/Write */
	[GDT_ENTRY_KERNEL_CS]		= GDT_ENTRY_INIT(0xc09a, 0, 0xfffff),
	[GDT_ENTRY_KERNEL_DS]		= GDT_ENTRY_INIT(0xc092, 0, 0xfffff),
	
	[GDT_ENTRY_USER_CS]		= GDT_ENTRY_INIT(0xc0fa, 0, 0xfffff),
	[GDT_ENTRY_USER_DS]		= GDT_ENTRY_INIT(0xc0f2, 0, 0xfffff),

	[GDT_ENTRY_KERNEL_TSS]		= GDT_ENTRY_INIT(0xc092, 0, 0),
	[GDT_ENTRY_KERNEL_PERCPU]	= GDT_ENTRY_INIT(0xc092, 0, 0xfffff),
};

struct desc_struct idt_table[IDT_ENTRIES] __aligned(8);

/* Prepare the screen, so we chould use printk()... */
void __init early_arch_setup(void)
{
	screen_info = boot_params.screen_info;
}

/*
 * The real arch setup...
 * x86 Architecture-specific boot-time initializations
 */
void __init arch_setup(void)
{
	setup_memory_map();

	max_pfn = e820_end_of_ram_pfn();

#ifdef CONFIG_PCI
	early_dump_pci_devices();
#endif

	//trap_init();
}
