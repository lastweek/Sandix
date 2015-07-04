#include <sandix/types.h>
#include <sandix/string.h>
#include <asm/bootparam.h>
#include <asm/e820.h>

extern struct boot_params boot_params;

/* System Memory Map */
struct e820map e820map;

void machine_specific_memory_setup(void)
{
	struct e820entry *entry, *dst, *src;
	
	/* copy e820 table */
	dst = &e820map.map;
	src = &boot_params.e820_map;
	memcpy(dst, src, sizeof(struct e820entry) * E820MAX);
	e820map.nr_entries = boot_params.e820_nr_entries;
}

