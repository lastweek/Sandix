#include <sandix/bootparam.h>
#include <sandix/const.h>
#include <sandix/e820.h>
#include <sandix/screen_info.h>
#include <sandix/segment.h>
#include <sandix/types.h>

/* %esi points to boot_params struct. */
struct boot_params boot_params;

/**
 * GDT contains 32 descriptors.
 **/
u64 gdt_table[GDT_ENTRIES] __attribute__((aligned(16))) =
{
	[GDT_ENTRY_KERNEL_CS]	= GDT_ENTRY(0xc09a, 0, 0xfffff),
	[GDT_ENTRY_KERNEL_DS]	= GDT_ENTRY(0xc092, 0, 0xfffff),
	
	[GDT_ENTRY_USER_CS]		= GDT_ENTRY(0xc0fa, 0, 0xfffff),
	[GDT_ENTRY_USER_DS]		= GDT_ENTRY(0xc0f2, 0, 0xfffff),

	[GDT_ENTRY_PERCPU]		= GDT_ENTRY(0xc092, 0, 0xfffff),
};

/**
 * IDT contains 256 gate decriptors.
 **/
u64 idt_table[IDT_ENTRIES] __attribute((aligned(16))) = {0};

void kernel_init(void)
{

}
