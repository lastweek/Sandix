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

/* Disable all interrupts at the legacy PIC. */
static void mask_all_interrupts(void)
{
	outb(0xff, 0xa1);	/* Mask all interrupts on the secondary PIC */
	io_delay();
	outb(0xfb, 0x21);	/* Mask all but cascade on the primary PIC */
	io_delay();
}

struct gdt_ptr {
	u16 len;
	u32 ptr;
} __attribute__((packed));

/* 
 *	(See description in section 3.4.5 Segment descriptors.)
 *
 *	CS: code, read/execute, 4 GB, base 0
 *	DS: data, read/write, 4 GB, base 0 
 *	TSS: 32-bit tss, 104 bytes, base 4096
 *	We only have a TSS here to keep Intel VT happy;
 *	we don't actually use it for anything.
*/
static void setup_gdt(void)
{
	/* Intel recommends 16 byte alignment. */
	static const u64 boot_gdt[] __attribute__((aligned(16))) = {
		[GDT_ENTRY_BOOT_CS]  = GDT_ENTRY(0xc09b, 0, 0xfffff),
		[GDT_ENTRY_BOOT_DS]  = GDT_ENTRY(0xc093, 0, 0xfffff),
		[GDT_ENTRY_BOOT_TSS] = GDT_ENTRY(0x0089, 4096, 103),
	};
	static struct gdt_ptr gdt;

	gdt.len = sizeof(boot_gdt)-1;
	gdt.ptr = (u32)&boot_gdt + (ds() << 4);

	asm volatile("lgdtl %0" : : "m" (gdt));
}

static void setup_idt(void)
{
	static const struct gdt_ptr null_idt = {0, 0};
	asm volatile("lidtl %0" : : "m" (null_idt));
}

void go_to_protected_mode(void)
{	
	/* Turn off interrupts */
	asm volatile("cli");
	mask_all_interrupts();/* why? */
	
	setup_idt();
	setup_gdt();
	
	/* Jump protected mode at 0x100000 */
	protected_mode_jump(0x100000, NULL);
}
