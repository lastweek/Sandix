/*
 *	Copyright (C) 2015-2016 Yizhou Shan <shanyizhou@ict.ac.cn>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License along
 *	with this program; if not, write to the Free Software Foundation, Inc.,
 *	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "boot.h"

#include <asm/bootparam.h>
#include <sandix/screen_info.h>

static unsigned short video_segment;

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

/*
 * Store the video mode parameters for later usage by the kernel.
 * This is done by asking the BIOS except for the rows/columns
 * parameters in the default 80x25 mode -- these are set directly,
 * because some very obscure BIOSes supply insane values.
 */
static void store_mode_params(void)
{
	u16 font_size;
	int x, y;

	store_cursor_position();
	store_video_mode();

	if (boot_params.screen_info.orig_video_mode == 0x07) {
		/* MDA, HGC, or VGA in monichrome mode */
		video_segment = 0xb000;
	} else {
		/* CGA, EGA, VGA and so forth */
		video_segment = 0xb800;
	}

	set_fs(0x0);
	font_size = rdfs16(0x485); /* Font size, BIOS Data Area */
	boot_params.screen_info.orig_video_points = font_size;

	x = rdfs16(0x44a);
	y = rdfs8(0x484)+1;
	boot_params.screen_info.orig_video_cols  = x;
	boot_params.screen_info.orig_video_lines = y;
}

static void save_screen(void)
{

}

static void restore_screen(void)
{

}

void set_video(void)
{
	unsigned short mode = boot_params.hdr.vid_mode;

	store_mode_params();
	save_screen();

	/* FIXME Should not print */
	if (boot_params.screen_info.orig_video_mode != 0x3) {
		set_video_mode(0x3);
		puts("Warning: Original video is not mode 3\n");
	}
}
