#include <sandix/types.h>
#include <sandix/page.h>
#include <sandix/mm.h>

unsigned long max_low_pfn;
unsigned long min_low_pfn;
unsigned long max_pfn;

void init_memory(void)
{
	setup_memory_map();
}
