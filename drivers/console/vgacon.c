/*
 *	Copyright (C) 2015-2016 Yizhou Shan <shan13@purdue.edu>
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

/*
 *	Low-Level VGA Console Driver
 *
 *	This is a simple driver for VGA video card. In a really system,
 *	every virtual console *must* have a in-memory buffer. Because many
 *	consoles may coexist in the meantime, so *no* one is allowed to talk
 *	to memory-mapped area(0xb8000) directly. Also, VGA card only have
 *	one set of hardware registers. Therefore, every time user switch to
 *	another console(ALT+CTL), system must save current VGA status, such
 *	as visible base address, cursor position, and then restore the status
 *	of the newly switched console(e.g. VGA_save(), VGA_restore()).
 *
 *	The bad news is: Sandix has not implement that.
 *	For simplicity, the vitual console layer and low-level console driver
 *	layer simply live under One-Console mode! No in-memory buffer, no
 *	VGA_save or VGA_restore. The VGA console driver talks to screen buffer
 *	directly!
 */

#include <sandix/io.h>
#include <sandix/irq.h>
#include <sandix/errno.h>
#include <sandix/types.h>
#include <sandix/console.h>
#include <sandix/compiler.h>
#include <sandix/screen_info.h>

#include <asm/bootparam.h>

#include <video/video.h>
#include <video/vga.h>

/****************************************************************************/
/*			VGA Driver Private Variables			    */
/****************************************************************************/

static	unsigned long	vga_visible_origin;	/* Upper left character */
static	unsigned long	vga_vram_base;		/* Base of video memory */
static	unsigned long	vga_vram_end;		/* End of video memory */
static	unsigned long	vga_vram_size;		/* Size of video memory */
static	unsigned long	vga_pos;		/* Cursor position address */
static	unsigned int	vga_x;			/* Cursor x */
static	unsigned int	vga_y;			/* Cursor y */
static  unsigned int	vga_vram_attr;		/* Character attribute */
static	unsigned short	vga_erased_char;	/* Erase background */
static	unsigned int	vga_video_num_cols;	/* Number of text columns */
static	unsigned int	vga_video_num_rows;	/* Number of text rows */
static	unsigned int	vga_video_port_reg;	/* Video register index port */
static	unsigned int	vga_video_port_val;	/* Video register value port */
static	int		vga_can_do_color;	/* Can we support colors? */
static	unsigned char	vga_video_type;		/* Card type */
const static unsigned char *vga_video_desc;	/* Card short description */

#define BLANK		(0x0020)

#define VGA_OFFSET(vc, y, x)	(unsigned long)(((vc)->vc_cols*(y)+(x))<<1)
#define VGA_ADDR(vc, y, x)	(unsigned long)((vc)->vc_visible_origin + VGA_OFFSET((vc), (y), (x)))

#define VGA_ATTR(vc, ch)	(unsigned short)((((vc)->vc_attr) << 8) | (unsigned char)(ch))

static __always_inline void write_vga(unsigned char reg, unsigned int val)
{
	outb(reg, vga_video_port_reg);
	outb(val, vga_video_port_val);
}

static __always_inline void vga_set_mem_top(struct vc_struct *vc)
{
	unsigned long offset = vc->vc_visible_origin - vga_vram_base;

	irq_disable();
	write_vga(VGA_CRTC_START_HI, 0xff&(offset >> 9));
	write_vga(VGA_CRTC_START_LO, 0xff&(offset >> 1));
	irq_enable();
}

static __always_inline void vga_set_cursor(struct vc_struct *vc)
{
	unsigned long offset = vc->vc_pos - vga_vram_base;

	irq_disable();
	write_vga(VGA_CRTC_CURSOR_HI, 0xff&(offset >> 9));
	write_vga(VGA_CRTC_CURSOR_LO, 0xff&(offset >> 1));
	irq_enable();
}

static void vgacon_cursor(struct vc_struct *vc, int mode)
{
	/* Fall-through, draw method only. */
	switch (mode) {
	case CM_ERASE:
	case CM_MOVE:
	case CM_DRAW:
		vga_set_cursor(vc);
		break;
	};
}

static void vgacon_set_origin(struct vc_struct *vc)
{
	vc->vc_origin = vc->vc_visible_origin = vga_vram_base;
	vga_set_mem_top(vc);
}

static void vgacon_putc(struct vc_struct *vc, int ch, int y, int x)
{
	unsigned long ADDR;

	ADDR = VGA_ADDR(vc, y, x);
	scr_writew(VGA_ATTR(vc, ch), ADDR);
}

static void vgacon_putcs(struct vc_struct *vc, const unsigned char *s,
			 int count, int y, int x)
{
	unsigned long ADDR;

	ADDR = VGA_ADDR(vc, y, x);
	for (; count > 0; count--) {
		scr_writew(VGA_ATTR(vc, *s++), ADDR);
		ADDR += 2;
	}
}

/* Clear Vertical Area */
static void vgacon_clear(struct vc_struct *vc, int y, int x,
			 int height, int width)
{
	unsigned long dest = VGA_ADDR(vc, y, x);
	unsigned short ech = VGA_ATTR(vc, vc->vc_erased_char);

	if (height <= 0 || width <= 0)
		return;
	
	for (; height > 0; height--) {
		scr_memsetw((unsigned short *)dest, ech, width);
		dest += vc->vc_row_size;
	}

}

static void vgacon_scroll(struct vc_struct *vc, int dir, int lines)
{
	unsigned long delta;

	if (lines <= 0 || lines >= vc->vc_rows / 2)
		return;
	
	delta = lines * vc->vc_row_size;

	switch (dir) {
	case (VWIN_UP):
		if ((vc->vc_visible_origin - delta) <= vga_vram_base) {
			vc->vc_visible_origin = vga_vram_base;
			vc->vc_scr_end = vga_vram_base + vc->vc_scr_size;
		} else {
			vc->vc_visible_origin -= delta;
			vc->vc_scr_end -= delta;
		}
		break;
	
	case (VWIN_DOWN):
		if ((vc->vc_scr_end + delta) <= vga_vram_end) {
			vc->vc_visible_origin += delta;
			vc->vc_scr_end += delta;
		} else {
			scr_memcpyw((u16 *)vga_vram_base,
				    (u16 *)vc->vc_visible_origin,
				    delta/2);
			vc->vc_pos = vga_vram_base
				+ (vc->vc_pos - vc->vc_visible_origin);
			vc->vc_visible_origin = vga_vram_base;
			vc->vc_scr_end = vga_vram_base + vc->vc_scr_size;
			
			/* Draw cursor */
			vga_set_cursor(vc);
		}
		break;
	};
	
	vga_set_mem_top(vc);
}

static bool has_called_startup = false;

/*
 * This function should be called only once.
 * Get basic information from "zeropage" to initialize
 * this vga_console module.
 */
static void vgacon_startup(void)
{
	if(has_called_startup)
		return;
	has_called_startup = true;

	/* boot_params.screen_info initialized? */
	if ((screen_info.orig_video_mode  == 0) &&
	    (screen_info.orig_video_lines == 0) &&
	    (screen_info.orig_video_cols  == 0))
	    	return;

	switch(screen_info.orig_video_mode) {
	case 7:
		/* Monochrome Display */
		vga_vram_base = 0xb0000;
		vga_video_port_reg = VGA_CRT_IM;
		vga_video_port_val = VGA_CRT_DM;
		
		/* See video-vga.c */
		if ((screen_info.orig_video_ega_bx & 0xff) != 0x10) {
			vga_video_desc = "EGA+";
			vga_video_type = VIDEO_TYPE_EGAM;
			vga_vram_size = 0x8000;
		} else {
			vga_video_desc = "*MDA";
			vga_video_type = VIDEO_TYPE_MDA;
			vga_vram_size = 0x2000;
		}
		break;
	default:
		/* Color Display */
		vga_can_do_color = 1;
		vga_vram_base = 0xb8000;
		vga_video_port_reg = VGA_CRT_IC;
		vga_video_port_val = VGA_CRT_DC;

		/* EGA? VGA? CGA? */
		if ((screen_info.orig_video_ega_bx & 0xff) != 0x10) {

			vga_vram_size = 0x8000;

			if (!screen_info.orig_video_isVGA) {
				/* EGA! */
				vga_video_desc = "EGA";
				vga_video_type = VIDEO_TYPE_EGAC;
			} else {
				/* VGA! */
				vga_video_desc = "VGA+";
				vga_video_type = VIDEO_TYPE_VGAC;
				/*XXX resize vram maybe? */
			}
		} else {
			/* CGA! */
			vga_video_desc = "*CGA";
			vga_vram_size = 0x2000;
		}
		break;
	}

	/* Calculate the final physical vram address */
	vga_vram_base = (unsigned long)phys_to_virt(vga_vram_base);
	vga_vram_end = vga_vram_base + vga_vram_size;

	vga_vram_attr = 0x7;
	vga_erased_char = (vga_vram_attr << 8) | BLANK;
	
	vga_visible_origin = vga_vram_base;

	vga_video_num_cols = screen_info.orig_video_cols;
	vga_video_num_rows = screen_info.orig_video_lines;

	vga_x = screen_info.orig_x;
	vga_y = screen_info.orig_y;
	vga_pos = (unsigned long)(((vga_video_num_cols * vga_y) + vga_x) << 1) + vga_vram_base;
}

/* 
 * This functions is called everytime VC starts a new console. It is used to
 * initialize the new vc_struct. Note that, if it is the first time to call
 * vgacon_init, vgacon_startup must be called first. And also, in real system,
 * a big memory area should be allocated for this new virtual console.
 */
static int vgacon_init(struct vc_struct *vc)
{
	if (!has_called_startup)
		return -EPERM;

	vc->vc_x	= vga_x;
	vc->vc_y	= vga_y;
	vc->vc_pos	= vga_pos;
	vc->vc_cols	= vga_video_num_cols;
	vc->vc_rows	= vga_video_num_rows;
	vc->vc_row_size	= vga_video_num_cols << 1;
	vc->vc_scr_size = vc->vc_row_size * vc->vc_rows;
	vc->vc_attr	= vga_vram_attr;
	vc->vc_origin	= vga_vram_base;
	vc->vc_scr_end	= vga_visible_origin + vc->vc_scr_size;
	vc->vc_top	= 0;
	vc->vc_bottom	= vga_video_num_rows;
	vc->vc_visible_origin = vga_visible_origin;
	vc->vc_erased_char = vga_erased_char;
	
	return 0;
}

/* free memory maybe */
static void vgacon_deinit(struct vc_struct *vc)
{
	return;
}

/*
 * vgacon_set_color
 * 00H  black  01H blue     02H  green  03H  cyan
 * 04H  red    05H magenta  06H  brown  07H  white
 */
static void vgacon_set_color(struct vc_struct *vc, int blink, int bg, int fg)
{
	if (bg > 7 || fg > 7)
		return;

	vc->vc_attr = (bg << 4) | fg;

	if (blink)
		vc->vc_attr |= 0x80;
}

const struct con_driver vga_con = {
	.con_startup	=	vgacon_startup,
	.con_init	=	vgacon_init,
	.con_deinit	=	vgacon_deinit,
	.con_clear	=	vgacon_clear,
	.con_putc	=	vgacon_putc,
	.con_putcs	=	vgacon_putcs,
	.con_cursor	=	vgacon_cursor,
	.con_scroll	=	vgacon_scroll,
	.con_set_color	=	vgacon_set_color,
	.con_set_origin	=	vgacon_set_origin
};
