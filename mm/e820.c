#include <sandix/types.h>
#include <sandix/string.h>
#include <sandix/page.h>
#include <asm/bootparam.h>
#include <asm/e820.h>

extern struct boot_params boot_params;

/* PC System Physical Memory Map */
struct e820map e820;

void machine_specific_setup_memory_map(void)
{
	struct e820entry *entry, *dst, *src;
	
	dst = &e820.map;
	src = &boot_params.e820_map;
	memcpy(dst, src, sizeof(struct e820entry) * E820MAX);
	e820map.nr_map = boot_params.e820_nr_entries;
}

