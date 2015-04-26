/*
 *	2015/04/24 Created by Shan Yizhou.
 *
 *	video.c: Set video mode. Store parameters.
 */

#include "boot.h"
//#include <sandix/bootparam.h>
//#include <sandix/screen_info.h>

static u8 get_video_mode_bios(void)
{
	struct biosregs ireg, oreg;

	initregs(&ireg);
	ireg.ah = 0x0F;
	intcall(0x10, &ireg, &oreg);
	
	return oreg.al;	
}

static void set_video_mode_bios(void)
{
	struct biosregs ireg;

	initregs(&ireg);
	ireg.al = 0x03; /* Mode 3 */
	intcall(0x10, &ireg, NULL);
}

void set_video(void)
{
	u8 mode;
	u8 rows;
	u16 cols;
	
	mode = get_video_mode_bios();
	if (mode != 3 && mode != 7)
		set_video_mode_bios();

	/* Get cols and rows from BDA */
	set_fs(0x0000);
	rows = rdfs8(0x484) + 1;
	cols = rdfs16(0x44A);
	printf("DEBUG: Video mode=%d, cols=%d, rows=%d\n", mode, cols, rows);
}
