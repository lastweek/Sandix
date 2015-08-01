/*
 *	init/main.c - Kernel Initialization
 *
 *	Copyright (C) 2015 Yizhou Shan
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
struct screen_info screen_info;

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

void kernel_init(void)
{
	struct tty_struct tty;
	int i;

	screen_info = boot_params.screen_info;
	con_init();
	
	FG_CON->con_set_color(FG_VC, 0, 1, 5);
	tty.console = FG_VC;

	con_write(&tty, "Hello! This is Sandix Kernel!\n\r~~~", 34);

	//FG_CON->con_scroll(FG_VC, 2, 4);
	//FG_CON->con_scroll(FG_VC, 1, 10);
	hlt();
}
