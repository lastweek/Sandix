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

#include <sandix/io.h>
#include <sandix/types.h>
#include <sandix/sched.h>
#include <sandix/kernel.h>
#include <sandix/export.h>
#include <sandix/bootmem.h>
#include <sandix/memblock.h>
#include <sandix/screen_info.h>

#include <asm/page.h>
#include <asm/e820.h>
#include <asm/traps.h>
#include <asm/mpspec.h>
#include <asm/pgtable.h>
#include <asm/segment.h>
#include <asm/x86_init.h>
#include <asm/sections.h>
#include <asm/bootparam.h>
#include <asm/processor.h>
#include <asm/pci_early.h>
#include <asm/descriptor.h>
#include <asm/early_ioremap.h>

/*
 * max_low_pfn_mapped: highest direct mapped pfn under 4GB
 * max_pfn_mapped:     highest direct mapped pfn over 4GB
 *
 * The direct mapping only covers E820_RAM regions, so the ranges and gaps are
 * represented by pfn_mapped
 */
unsigned long max_low_pfn_mapped;
unsigned long max_pfn_mapped;

/*
 * _brk_end equals to _brk_start at first place, which means no brk is allocated.
 * The first portion of __brk area is used by intial page tables. Other code
 * will allocate brk via extend_brk. However, remember, we do have a _brk_limit!
 */
unsigned long _brk_start = (unsigned long)__brk_start;
unsigned long _brk_end = (unsigned long)__brk_start;

/* Machine parameters obtained from BIOS */
struct boot_params boot_params;
EXPORT_SYMBOL(boot_params);

/* Used by low-level console drivers */
struct screen_info screen_info;
EXPORT_SYMBOL(screen_info);

static struct resource code_resource = {
	.name	= "Kernel Code",
	.start	= 0,
	.end	= 0,
	.flags	= IORESOURCE_BUSY | IORESOURCE_MEM
};
static struct resource data_resource = {
	.name	= "Kernel Data",
	.start	= 0,
	.end	= 0,
	.flags	= IORESOURCE_BUSY | IORESOURCE_MEM
};
static struct resource bss_resource = {
	.name	= "Kernel Bss",
	.start	= 0,
	.end	= 0,
	.flags	= IORESOURCE_BUSY | IORESOURCE_MEM
};

/* Legacy IO port distribution */
static struct resource standard_io_resources[] = { {
	.name	= "dma1",
	.start	= 0x00,
	.end	= 0x1f,
	.flags	= IORESOURCE_BUSY | IORESOURCE_IO
} , {
	.name	= "pci1",
	.start	= 0x20,
	.end	= 0x21,
	.flags	= IORESOURCE_BUSY | IORESOURCE_IO
} , {
	.name	= "timer0",
	.start	= 0x40,
	.end	= 0x43,
	.flags	= IORESOURCE_BUSY | IORESOURCE_IO
} , {
	.name	= "timer1",
	.start	= 0x50,
	.end	= 0x53,
	.flags	= IORESOURCE_BUSY | IORESOURCE_IO
} , {
	.name	= "keyboard",
	.start	= 0x60,
	.end	= 0x60,
	.flags	= IORESOURCE_BUSY | IORESOURCE_IO
} , {
	.name	= "keyboard",
	.start	= 0x64,
	.end	= 0x64,
	.flags	= IORESOURCE_BUSY | IORESOURCE_IO
} , {
	.name	= "dma page reg",
	.start	= 0x80,
	.end	= 0x8f,
	.flags	= IORESOURCE_BUSY | IORESOURCE_IO
} , {
	.name	= "pic2",
	.start	= 0xa0,
	.end	= 0xa1,
	.flags	= IORESOURCE_BUSY | IORESOURCE_IO
} , {
	.name	= "dma2",
	.start	= 0xc0,
	.end	= 0xdf,
	.flags	= IORESOURCE_BUSY | IORESOURCE_IO
} , {
	.name	= "fpu",
	.start	= 0xf0,
	.end	= 0xff,
	.flags	= IORESOURCE_BUSY | IORESOURCE_IO
} };

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

void * __init extend_brk(unsigned long size, unsigned long align)
{
	unsigned long mask = align - 1;
	void *ret;

	/* brk is closed */
	BUG_ON(_brk_start == 0);

	/* align must be 2-order aligned */
	BUG_ON(align & mask);

	_brk_end = (_brk_end + mask) & ~mask;

	/* use a larger brk if possible */
	BUG_ON((char *)(_brk_end + size) > __brk_limit);

	ret = (void *)_brk_end;
	_brk_end += size;

	memset(ret, 0, size);

	return ret;
}

static void __init memblock_reserve_brk(void)
{
	if (_brk_end > _brk_start)
		memblock_reserve(__pa(_brk_start),
				 _brk_end - _brk_start);

	/*
	 * Mark brk area as locked down and no longer taking
	 * any new allocations.
	 */
	_brk_start = 0;
}

/* Prepare screen first */
void __init early_arch_setup(void)
{
	screen_info = boot_params.screen_info;
}

/*
 * setup_arch - architecture-specific boot-time initializations
 *
 * Note: On x86_64, fixmaps are ready for use even before this is called.
 */

/*
 * TODO:
 * 1)
 * Desktop Management Interface (DMI), which scan different hardware
 * components within the laptop, desktop or server machines. We need
 * to add drivers/firmware/dmi_scan.c to support this.
 *
 * 2)
 * Implement find_smp_config(), which is mpspec.c that parse mp table.
 */

void __init arch_setup(void)
{
	int early_dump_pci = 1;

        memblock_reserve(__pa(__kstart),
		(unsigned long)__bss_end - (unsigned long)__kstart);

	/*
	 * At this point everything still needed from the boot loader
	 * or BIOS or kernel text should be early reserved or marked not
	 * RAM in e820. All other memory is free game.
	 */

	early_cpu_init();
	early_ioremap_init();

	x86_configure_nx();
	x86_report_nx();

	iomem_resource.end = (1ULL << boot_cpu_info.x86_phys_bits) - 1;
	setup_memory_map();

	init_mm.start_code	= (unsigned long)__text_start;
	init_mm.start_data	= (unsigned long)__data_start;
	init_mm.start_brk	= (unsigned long)__brk_start;
	init_mm.end_code	= (unsigned long)__text_end;
	init_mm.end_data	= (unsigned long)__data_end;
	init_mm.brk		= _brk_end;

	code_resource.start	= __pa(__text_start);
	code_resource.end	= __pa(__text_end) - 1;
	data_resource.start	= __pa(__data_start);
	data_resource.end	= __pa(__data_end) - 1;
	bss_resource.start	= __pa(__bss_start);
	bss_resource.end	= __pa(__bss_end) - 1;

	insert_resource(&iomem_resource, &code_resource);
	insert_resource(&iomem_resource, &data_resource);
	insert_resource(&iomem_resource, &bss_resource);

	x86_init.oem.arch_setup();
	x86_init.resources.probe_roms();

#ifdef CONFIG_PCI
	if (early_dump_pci)
		early_dump_pci_devices();
#endif

	/*
	 * partially used pages are not usable - thus
	 * we are rounding upwards:
	 */
	max_pfn = e820_end_of_ram_pfn();

#ifdef CONFIG_X86_32
	/* max_low_pfn get updated here */
	find_low_pfn_range();
#else
	if (max_pfn > (1UL << (32 - PAGE_SHIFT)))
		max_low_pfn = e820_end_of_low_ram_pfn();
	else
		max_low_pfn = max_pfn;

	high_memory = (void *)__va(max_pfn * PAGE_SIZE - 1) + 1;
#endif

	/*
	 * Find and reserve possible boot-time SMP configuration:
	 */
	find_smp_config();

	early_alloc_pgt_buf();

	/*
	 * Need to conclude brk, before memblock_x86_fill(). It could use
	 * memblock_find_in_range, could overlap with brk area.
	 *
	 * Note that no extend_brk is allowed to use afterwards.
	 */
	memblock_reserve_brk();

	/*
	 * Add all E820_RAM into logical memblocks,
	 * and dump all memory+reserved memblocks.
	 */
	memblock_x86_fill();
	memblock_dump_all();

#ifdef CONFIG_X86_32
	printk(KERN_DEBUG "initial memory mapped: [mem 0x00000000-%#010lx]\n",
		(max_pfn_mapped<<PAGE_SHIFT) - 1);
#endif

	init_mem_mapping();

	reserve_standard_io_resources();
}
