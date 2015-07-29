#include <sandix/const.h>
#include <sandix/mm.h>
#include <sandix/string.h>
#include <sandix/screen_info.h>
#include <sandix/types.h>

#include <asm/bootparam.h>
#include <asm/segment.h>
#include <asm/descriptor.h>

struct boot_params boot_params;
struct screen_info screen_info;

#define __ALIGN8__	__attribute__((aligned(8)))

/* NOTE! accessed type useless, We do not use! */
struct desc_struct gdt_table[GDT_ENTRIES] __ALIGN8__ =
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

struct desc_struct idt_table[IDT_ENTRIES] __ALIGN8__;

void handle_int(void)
{
	asm volatile (
		"incb 0xb8000\n\t"
		"incb 0xb8001\n\t"
		"iret"
	);
}

#define _VGA_OFFSET(x,y) (unsigned long) ((y*80+x)<<1)
#define _VGA_ATTR(c)	((c) | (0x7 << 8))
#define _scr_writew(v, a) (*(short *)a) = (short)v

 void _vgacon_putcs(const unsigned char *s, int count, int y, int x)
{
	int i;
	unsigned long ADDR;

	ADDR = 0xb8000 + _VGA_OFFSET(x, y);

	for (i = 0; i < count; i++) {
		_scr_writew(_VGA_ATTR(*s), ADDR);
		ADDR += 2;
		s++;
	}
}


void kernel_init(void)
{
	//init_memory();
	//init_traps();
}
