/*
 *	2015/04/21 Created by Shan Yizhou.
 *
 *	memory.c: Memory detection code.
 *
 *	Note: E820 is supported by nearly all BIOSes since 2002.
 *	It's not wise to bypass BIOS and detect memory yourself
 *	like what BIOS do. I don't konw if Linux detect memory
 *	again after protected-mode kernel starts. But i think
 *	Linux won't "believe" BIOS~~~
 */

#include "boot.h"
#include <e820.h>

#define SMAP	0x534D4150	/* ASCII "SMAP" */

int detect_memory_e820(void)
{
	int count = 0;
	struct biosregs ireg, oreg;
	struct e820entry buf;

	initregs(&ireg);
	ireg.ax  = 0xE820;
	ireg.bx  = 0;
	ireg.cx  = sizeof(struct e820entry);
	ireg.edx = SMAP;
	ireg.di  = (size_t)&buf;

	do {
		intcall(0x15, &ireg, &oreg);
		ireg.ebx = oreg.ebx; /* for next iteration */
		
		if (oreg.eflags & 1) {
			puts("DEBUG: ERROR detect memory fail! EFLAGS.CF = 1\n");
			break;
		}

		if (oreg.eax != SMAP) {
			count = 0;
			puts("DEBUG: ERROR detect memory fail! EAX != SMAP\n");
			break;
		}
		
		printf("Base: %X End: %X Len: %X Type: %d \n",
				buf.base_low, buf.base_low+buf.len_low, buf.len_low, buf.type);

	} while (ireg.ebx);
	
	return count;
}

int detect_memory_e801(void)
{
	struct biosregs ireg, oreg;

	initregs(&ireg);
	ireg.ax = 0xE801;
	intcall(0x15, &ireg, &oreg);

	if (oreg.eflags & 1) {
		puts("DEBUG: ERROR detect memory e801 CF fail\n");
		return -1;
	}
	
	printf("ax: %X bx: %X cx: %X dx: %X\n", oreg.ax, oreg.bx, oreg.cx, oreg.cx);

	return 0;
}

int detect_memory_88(void)
{

}

void detect_memory(void)
{
	detect_memory_e820();
	detect_memory_e801();
}

