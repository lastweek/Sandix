#include <sandix/const.h>
#include <sandix/types.h>
#include <sandix/bootparam.h>

#include <asm/segment.h>

struct boot_params boot_params;

u64 gdt_table[GDT_ENTRIES] __attribute__((aligned(16))) =
{
	/* NOTE! accessed type useless! So we do not use! */
	/* Present, DPL=0, Execute/Read */
	[GDT_ENTRY_KERNEL_CS]	= GDT_ENTRY(0xc09a, 0, 0xfffff),
	/* Present, DPL=0, Read/Write */
	[GDT_ENTRY_KERNEL_DS]	= GDT_ENTRY(0xc092, 0, 0xfffff),
	
	/* Present, DPL=3, Execute/Read */
	[GDT_ENTRY_USER_CS]		= GDT_ENTRY(0xc0fa, 0, 0xfffff),
	/* Present, DPL=3, Read/Write */
	[GDT_ENTRY_USER_DS]		= GDT_ENTRY(0xc0f2, 0, 0xfffff),

	//[GDT_ENTRY_KERNEL_TSS]	=
	//[GDT_ENTRY_KERNEL_LDT]
	
	[GDT_ENTRY_KERNEL_PERCPU]		= GDT_ENTRY(0xc092, 0, 0xfffff),
};

u64 idt_table[IDT_ENTRIES] __attribute__((aligned(16)));

void cpu_init(void)
{
	
}

/**
 * Yeah, here comes the init guy.
 **/
void kernel_init(void)
{
	cpu_init();
}
