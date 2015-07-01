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
#include <asm/e820.h>
#include <asm/bootparam.h>

#define SMAP	0x534D4150	/* ASCII "SMAP" */

static int detect_memory_e820(void)
{
	int count = 0;
	struct biosregs ireg, oreg;
	struct e820entry *desc = boot_params.e820_map;
	struct e820entry buf = {0,0,0};

	initregs(&ireg);
	ireg.ax  = 0xe820;
	ireg.cx  = sizeof(struct e820entry);
	ireg.edx = SMAP;
	ireg.di  = (size_t)&buf;

	do {
		intcall(0x15, &ireg, &oreg);
		ireg.ebx = oreg.ebx; /* for next iteration */
		
		if (oreg.eflags & 1) {
			count = 0;
			break;
		}

		/**
		 * Some BIOSes stop returning SMAP in the middle of
		 * the search loop.  We don't know exactly how the BIOS
		 * screwed up the map at that point, we might have a
		 * partial map, the full map, or complete garbage, so
		 * just return failure.
		 **/
		if (oreg.eax != SMAP) {
			count = 0;
			break;
		}
		
		*desc++ = buf;
		count++;
	} while (ireg.ebx && count < E820MAX);
	
	return boot_params.e820_entries_nr = count;
}

void detect_memory(void)
{
	int err = detect_memory_e820();
	
	if (!err)
		puts("DEBUG: Get e820 Map ... FAIL\n");
}
