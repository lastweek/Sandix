/*
 *	2014/04/27 Created by Shan Yizhou.
 *	
 *	pm.c: Prepare the machine for transition to protected mode.
 *
 *	Note: The minimum requirement data structures and registers
 *	needed for transition to protected mode is described in
 *	Intel-Software-Developer-manual-Vol-3, Chapter 9, PROCESSOR
 *	MANAGEMENT AND INITIALIZATION.
 */

#include "boot.h"
#include <sandix/segment.h>

/* 
 *	CS  code, read/execute, 4 GB, base 0
 *	DS  data, read/write, 4 GB, base 0 
 *	TSS 32-bit tss, 104 bytes, base 4096
 *		Have a TSS here to keep Intel VT happy;
 *		we don't use it for anything.
*/
static const u64 boot_gdt[] __attribute__((aligned(16))) = {
	/* Intel recommends 16 byte alignment. */
	[GDT_ENTRY_BOOT_CS]  = GDT_ENTRY(0xc09b, 0, 0xfffff),
	[GDT_ENTRY_BOOT_DS]  = GDT_ENTRY(0xc093, 0, 0xfffff),
	[GDT_ENTRY_BOOT_TSS] = GDT_ENTRY(0x0089, 4096, 103),
};

struct gdt_ptr {
	u16 len;
	u32 ptr;
} __attribute__((packed));

static struct gdt_ptr gdt = {1, 1};
static struct gdt_ptr idt = {1, 1};


static void setup_gdt(void)
{
	gdt.len = sizeof(boot_gdt)-1;
	gdt.ptr = (u32)&boot_gdt + 0x90000;
	asm volatile("lgdtl %0" : : "m" (gdt));
}

static void setup_idt(void)
{
	idt.len = 0;
	idt.ptr = 0;
	//asm volatile("lidtl %0" : : "m" (idt));
}

void go_to_protected_mode(void)
{
	asm volatile("cli");
	setup_idt();
	setup_gdt();
	
	/**
	 * Go to PM, never return
	 **/
	protected_mode_jump((u32)0x10000, (u32)&boot_params + 0x90000);
}


