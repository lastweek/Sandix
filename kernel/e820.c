#include <sandix/types.h>
#include <sandix/string.h>
#include <asm/bootparam.h>
#include <asm/e820.h>
#include <asm/page.h>

extern struct boot_params boot_params;

/* PC System Physical Memory Map */
struct e820map e820;

void machine_specific_memory_setup(void)
{
	struct e820entry *entry, *dst, *src;
	
	/* copy e820 table */
	dst = &e820.map;
	src = &boot_params.e820_map;
	memcpy(dst, src, sizeof(struct e820entry) * E820MAX);
	e820map.nr_map = boot_params.e820_nr_entries;
}

