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

#include <sandix/io.h>
#include <sandix/types.h>
#include <sandix/kernel.h>
#include <sandix/export.h>
#include <sandix/bootmem.h>
#include <sandix/compiler.h>
#include <sandix/screen_info.h>

#include <asm/e820.h>
#include <asm/traps.h>
#include <asm/segment.h>
#include <asm/sections.h>
#include <asm/bootparam.h>
#include <asm/processor.h>
#include <asm/pci_early.h>
#include <asm/descriptor.h>

unsigned long max_low_pfn_mapped;
unsigned long max_pfn_mapped;

unsigned long brk_start = (unsigned long)__brk_start;
unsigned long brk_end = (unsigned long)__brk_start;

/* machine parameters obtained from BIOS */
struct boot_params boot_params;
EXPORT_SYMBOL(boot_params);

/* used by low-level console drivers */
struct screen_info screen_info;
EXPORT_SYMBOL(screen_info);

struct cpuinfo_x86 boot_cpu_data __read_mostly;
EXPORT_SYMBOL(boot_cpu_data);

/* kernel image... */
static struct resource data_resource = {
	.name	= "kernel data",
	.start	= 0,
	.end	= 0,
	.flags	= IORESOURCE_BUSY | IORESOURCE_MEM
};
static struct resource code_resource = {
	.name	= "kernel code",
	.start	= 0,
	.end	= 0,
	.flags	= IORESOURCE_BUSY | IORESOURCE_MEM
};
static struct resource bss_resource = {
	.name	= "kernel bss",
	.start	= 0,
	.end	= 0,
	.flags	= IORESOURCE_BUSY | IORESOURCE_MEM
};

/* legacy io port distribution */
static struct resource standard_io_resources[] = {
	{
		.name	= "dma1",
		.start	= 0x00,
		.end	= 0x1f,
		.flags	= IORESOURCE_BUSY | IORESOURCE_IO
	} ,
	{
		.name	= "pci1",
		.start	= 0x20,
		.end	= 0x21,
		.flags	= IORESOURCE_BUSY | IORESOURCE_IO
	} ,
	{
		.name	= "timer0",
		.start	= 0x40,
		.end	= 0x43,
		.flags	= IORESOURCE_BUSY | IORESOURCE_IO
	} ,
	{
		.name	= "timer1",
		.start	= 0x50,
		.end	= 0x53,
		.flags	= IORESOURCE_BUSY | IORESOURCE_IO
	} ,
	{
		.name	= "keyboard",
		.start	= 0x60,
		.end	= 0x60,
		.flags	= IORESOURCE_BUSY | IORESOURCE_IO
	} ,
	{
		.name	= "keyboard",
		.start	= 0x64,
		.end	= 0x64,
		.flags	= IORESOURCE_BUSY | IORESOURCE_IO
	} ,
	{
		.name	= "dma page reg",
		.start	= 0x80,
		.end	= 0x8f,
		.flags	= IORESOURCE_BUSY | IORESOURCE_IO
	} ,
	{
		.name	= "pic2",
		.start	= 0xa0,
		.end	= 0xa1,
		.flags	= IORESOURCE_BUSY | IORESOURCE_IO
	} ,
	{
		.name	= "dma2",
		.start	= 0xc0,
		.end	= 0xdf,
		.flags	= IORESOURCE_BUSY | IORESOURCE_IO
	} ,
	{
		.name	= "fpu",
		.start	= 0xf0,
		.end	= 0xff,
		.flags	= IORESOURCE_BUSY | IORESOURCE_IO
	}
};

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

void __init reserve_standard_io_resources(void)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(standard_io_resources); i++)
		BUG_ON(!!request_resource(&ioport_resource, &standard_io_resources[i]));
}

/* prepare screen, then we can use printk */
void __init early_arch_setup(void)
{
	screen_info = boot_params.screen_info;
}

/* the real x86 architecture setup */
void __init arch_setup(void)
{
	setup_memory_map();

	max_pfn = e820_end_of_ram_pfn();

#ifdef CONFIG_PCI
	early_dump_pci_devices();
#endif

	reserve_standard_io_resources();
}
