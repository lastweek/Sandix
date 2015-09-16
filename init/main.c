/*

 *	init/main.c - Kernel Initialization
 *
 *	Copyright (C) 2015 Yizhou Shan <shanyizhou@ict.ac.cn>
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

#include <sandix/compiler.h>
#include <sandix/const.h>
#include <sandix/console.h>
#include <sandix/mm.h>
#include <sandix/string.h>
#include <sandix/screen_info.h>
#include <sandix/tty.h>
#include <sandix/types.h>

#include <asm/bootparam.h>
#include <asm/segment.h>
#include <asm/descriptor.h>

struct boot_params boot_params;
EXPORT_SYMBOL(boot_params);

struct screen_info screen_info;
EXPORT_SYMBOL(screen_info);

#define __ALIGN8	__attribute__((aligned(8)))

/* NOTE! accessed type useless, We do not use! */
struct desc_struct gdt_table[GDT_ENTRIES] __ALIGN8 =
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

struct desc_struct idt_table[IDT_ENTRIES] __ALIGN8;

void handle_int(void)
{
	asm volatile (
		"incb 0xb8000\n\t"
		"incb 0xb8001\n\t"
		"iret"
	);
}

void hlt(void)
{
	asm volatile (
		"1: hlt\n\t"
		"jmp 1b"
	);
}

void __init kernel_init(void)
{
	struct tty_struct tty;
	int i;
	const char *s = "\033[5mHello!34mABCl\033[0mBBB\n\r~~~";

	screen_info = boot_params.screen_info;
	tty_init();
	
	tty.console = FG_VC;
	con_write(&tty, s, strlen(s));
	
	hlt();
}
