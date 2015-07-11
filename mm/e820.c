#include <sandix/types.h>
#include <sandix/string.h>
#include <sandix/page.h>
#include <asm/bootparam.h>
#include <asm/e820.h>

struct e820map e820;

/*
 * Build up system memory map.
 */
void setup_memory_map(void)
{
	struct e820entry *entry, *dst, *src;
	
	dst = &(e820.map);
	src = &(boot_params.e820_map);
	memcpy((void *)dst, (void *)src, sizeof(struct e820entry) * E820MAX);
	e820.nr_entries = boot_params.e820_nr_entries;
}

