/*
 *	2015/04/24 Created by Shan Yizhou.
 *
 *	video.c: Set video mode.
 */

#include "boot.h"
#include <sandix/bootparam.h>
#include <sandix/screen_info.h>

static u8 get_video_mode_bios(void)
{
	struct biosregs ireg, oreg;

	initregs(&ireg);
	ireg.ah = 0x0F;
	intcall(0x10, &ireg, &oreg);
	
	return oreg.al;	
}

static void set_video_mode_bios(u8 mode)
{
	struct biosregs ireg;

	initregs(&ireg);
	ireg.al = mode; /* Mode 3 */
	intcall(0x10, &ireg, NULL);
}

/*
 *	1. Get current video mode, cols and rows. Set mode 3.
 *	2. Store video mode and video segment base address.
 *	3. Mode 3 VGA can be 80x25 or 80x50, so get rows from BDA.
 */
void set_video(void)
{
	u8 mode;
	u16 cols, rows;
	
	mode = get_video_mode_bios();
	if (mode != 3 && mode != 7)
		mode = 3;
	set_video_mode_bios(mode);

	/* Get cols and rows from BDA */

}



