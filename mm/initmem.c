/*
 *	Boot time system memory setup.
 */
#include <sandix/types.h>
#include <sandix/page.h>

unsigned long max_low_pfn;
unsigned long min_low_pfn;
unsigned long max_pfn;

void setup_memory_map(void)
{
	machine_specific_setup_memory_map();
}

void init_memory(void)
{
	setup_memory_map();
}
