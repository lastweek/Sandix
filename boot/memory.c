/*
 *	2015/04/21 Created by Shan Yizhou.
 *
 *	memory.c: Memory detection code.
 *
 *	Note: This code use E820 only. E801 and 88 are some
 *	kind of "obsolete" in 32-bit CPU. E820 is supported
 *	by nearly all BIOSes since 2002. It's not wise to
 *	bypass BIOS and detect memory yourself like what BIOS do. 
 */

#include "boot.h"
#include <e820.h>
//#include <bootparam.h>

#define SMAP	0x534D4150	/* ASCII "SMAP" */

int detect_memory_e820(void)
{
	int count = 0;
	struct biosregs ireg, oreg;
	struct e820entry buf;

	initregs(&ireg);
	ireg.ax  = 0xE820;
	ireg.cx  = sizeof(struct e820entry);
	ireg.edx = SMAP;
	ireg.di  = (size_t)&buf;

	do {
		intcall(0x15, &ireg, &oreg);
		ireg.ebx = oreg.ebx; /* for next iteration */
		
		/* Note by autor:
		   BIOSes which terminate the chain with CF = 1 as opposed
		   to %ebx = 0 don't always report the SMAP signature on
		   the final, failing, probe. */
		if (oreg.eflags & 1) {
			count = 0;
			puts("DEBUG: ERROR detect memory fail! EFLAGS.CF = 1\n");
			break;
		}

		/* Note by author:
		   Some BIOSes stop returning SMAP in the middle of
		   the search loop.  We don't know exactly how the BIOS
		   screwed up the map at that point, we might have a
		   partial map, the full map, or complete garbage, so
		   just return failure. */
		if (oreg.eax != SMAP) {
			count = 0;
			puts("DEBUG: ERROR detect memory fail! EAX != SMAP\n");
			break;
		}
		
		printf("Base: %X End: %X Len: %X Type: %d \n",
				buf.base_low, buf.base_low+buf.len_low, buf.len_low, buf.type);
		count++;

	} while (ireg.ebx);
	
	return count;
}

void detect_memory(void)
{
	if (!detect_memory_e820())
		puts("DEBUG: Detect Memory...FAIL\n");
	else
		puts("DEBUG: Detect Memory...OK\n");
}

