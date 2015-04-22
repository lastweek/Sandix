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

#define SMAP	0x534d4150	/* ASCII "SMAP" */

int detect_memory_e820(void)
{

}

int detect_memory_e801(void)
{
	
}

int detect_memory_88(void)
{

}

int detect_memory(void)
{
	int err = -1;

	return err;
}
