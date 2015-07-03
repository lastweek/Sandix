#include <sandix/types.h>
#include <sandix/string.h>
#include <asm/bootparam.h>
#include <asm/e820.h>

extern struct e820map e820map;
extern struct boot_params boot_params;

/* Using table e820 to build system memory map */
void machine_specific_memory_setup(void)
{
	struct e820entry *entry;
	
	/* Copy e820map */
	memcpy();
}

