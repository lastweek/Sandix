/*
 *	2015/04/24 Created by Shan Yizhou.
 *
 *	video.c: Set video mode.
 */

#include "boot.h"
#include <sandix/bootparam.h>
#include <sandix/screen_info.h>

static u8 get_video_mode_bios()
{
	struct biosregs ireg, oreg;

	initregs(&ireg);
	ireg.ah = 0x0F;
	intcall(0x10, &ireg, &oreg);
	
	return oreg.al;	
}

static void set_video_mode_bios()
{
	struct biosregs ireg;

	initregs(&ireg);
	ireg.ah = 0x0;
	ireg.al = 0x03; /* Mode 3 */
	intcall(0x10, &ireg, NULL);
}

void set_video(void)
{
	u8 mode;
	
	mode = get_video_mode_bios();
	if (mode != 3)
		set_video_mode_bios();
	puts("DEBUG: set video mode 3...OK\n");


}



