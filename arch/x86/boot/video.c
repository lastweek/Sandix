/*
 *	2015/04/24 Created by Shan Yizhou.
 *	video.c: Store parameters of screen and set video mode.
 */

#include "boot.h"
#include <asm/bootparam.h>
#include <sandix/screen_info.h>

static void set_video_mode(u8 mode)
{
	struct biosregs ireg;

	initregs(&ireg);
	ireg.al = mode; /* AH=00 Set Video Mode */
	intcall(0x10, &ireg, NULL);
}

static void store_cursor_position(void)
{
	struct biosregs ireg, oreg;

	initregs(&ireg);
	ireg.ah = 0x03;
	intcall(0x10, &ireg, &oreg);

	boot_params.screen_info.orig_x = oreg.dl;
	boot_params.screen_info.orig_y = oreg.dh;
}

static void store_video_mode(void)
{
	struct biosregs ireg, oreg;

	initregs(&ireg);
	ireg.ah = 0x0f;
	intcall(0x10, &ireg, &oreg);

	/* Not all BIOSes are clean with respect to the top bit */
	boot_params.screen_info.orig_video_mode = oreg.al & 0x7f;
	boot_params.screen_info.orig_video_page = oreg.bh;
}

static void store_mode_params(void)
{
	u16 font_size;
	int x, y;

	store_cursor_position();
	store_video_mode();

	set_fs(0x0);
	font_size = rdfs16(0x485); /* Font size, BIOS Data Area */
	boot_params.screen_info.orig_video_points = font_size;

	x = rdfs16(0x44a);
	y = rdfs8(0x484)+1; /* Convention */
	boot_params.screen_info.orig_video_cols  = x;
	boot_params.screen_info.orig_video_lines = y;
}

void set_video(void)
{
	store_mode_params();

	/* FIXME Should not print */
	if (boot_params.screen_info.orig_video_mode != 0x3) {
		set_video_mode(0x3);
		puts("Warning: Original video is not mode 3\n");
	}
}
