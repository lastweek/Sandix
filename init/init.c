#include <sandix/bootparam.h>
#include <sandix/const.h>
#include <sandix/e820.h>
#include <sandix/screen_info.h>
#include <sandix/segment.h>
#include <sandix/types.h>

/* %esi points to boot_params struct. */
struct boot_params boot_params;

u64 GDT_KERNEL[GDT_ENTRIES] __attribute__((aligned(16))) =
{
	[GDT_ENTRY_KERNEL_CS]	= GDT_ENTRY(0xc09b, 0, 0xfffff),
	[GDT_ENTRY_KERNEL_DS]	= GDT_ENTRY(0xc093, 0, 0xfffff),
	[GDT_ENTRY_USER_CS]		= GDT_ENTRY(0xc09b, 0, 0xfffff),
	[GDT_ENTRY_USER_DS]		= GDT_ENTRY(0xc093, 0, 0xfffff),
	[GDT_ENTRY_KERNEL_TSS]	= GDT_ENTRY(),
	[GDT_ENTRY_KERNEL_LDT]	= GDT_ENTRY(),
};

static void setup_gdt(void)
{

}

static void setup_idt(void)
{
	
}

void kernel_init(void)
{

}
