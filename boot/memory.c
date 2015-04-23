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
		
		if (oreg.eflags & 1){
			puts("DEBUG: ERROR detect memory fail! EFLAGS.CF = 1\n");
			break;
		}

		if (oreg.eax != SMAP) {
			count = 0;
			puts("DEBUG: ERROR detect memory fail! EAX != SMAP\n");
			break;
		}
	}while(ireg.ebx);
	
	return count;
}

int detect_memory_e801(void)
{
	
}

int detect_memory_88(void)
{

}

int _detect_memory(void)
{
	int err = -1;
	
	if (detect_memory_e820 > 0)
		err = 0;

	return err;
}

void detect_memory(void)
{
	if (!_detect_memory())
		printf("DEBUG: Memory detect...OK\n");
	else
		printf("DEBUG: Memory detect...FAIL\n");
}


