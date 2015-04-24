/*
 *
 *
 *
 */

#include "boot.h"

int get_current_video_mode_bios()
{
	struct biosregs ireg, oreg;

	initregs(&ireg);
	ireg.ah = 0x0F;
	intcall(0x10, &ireg, &oreg);
	
	return oreg.al;	/* al is unsigned char */
}

int get_current_page_bios()
{
	
}
