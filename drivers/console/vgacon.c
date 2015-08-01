/*
 *	Low-Level VGA-Based Console Driver.
 *
 *	Copyright (C) 2015 Yizhou Shan <shanyizhou@ict.ac.cn>
 *
 *	This is a simple driver for VGA video card. In a really system,
 *	every virtual console must have a in-memory buffer. Because many
 *	consoles may coexist in the meantime, so _no_ one is allowed to talk
 *	to memory-mapped area(0xb8000) directly. Also, VGA card only have
 *	one set of hardware registers. Therefore, every time USERs switch to
 *	another console, system must save current VGA status, such as visible
 *	base address, cursor position, first, and then restore the status of
 *	the newly switched console(like VGA_save(), VGA_restore()).
 *
 *	BUT, Sandix has not implement that for now, cause i do not need it.
 *	Only one VGA console is alive, just talk to hardware directly.
 *
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

#include <sandix/compiler.h>
#include <sandix/console.h>
#include <sandix/errno.h>
#include <sandix/irq.h>
#include <sandix/io.h>
#include <sandix/screen_info.h>
#include <sandix/types.h>
#include <asm/bootparam.h>
#include <video/vga.h>
#include <video/video.h>

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

/* WHY NOT? */
#define BLANK		(0x20)
#define SCREEN_SIZE	(4000)
#define ROW_SIZE	(160)

#define VGA_OFFSET(y, x)	(unsigned long)((80*(y)+(x))<<1)
#define VGA_ADDR(vc, y, x)	((vc)->vc_visible_origin + VGA_OFFSET((y), (x)))
#define VGA_ATTR(vc, ch)	(unsigned short)(((vc->vc_attr) << 8) | (ch))
#define VGA_MEM_MAP(__addr)	(unsigned long)phys_to_virt((__addr))

ALWAYS_INLINE void write_vga(unsigned char reg, unsigned int val)
{
	outb(reg, vga_video_port_reg);
	outb(val, vga_video_port_val);
}

INLINE void vga_set_mem_top(struct vc_struct *vc)
{
	unsigned long offset = vc->vc_visible_origin - vga_vram_base;

	irq_disable();
	write_vga(VGA_CRTC_START_HI, 0xff&(offset >> 9));
	write_vga(VGA_CRTC_START_LO, 0xff&(offset >> 1));
	irq_enable();
}

INLINE void vga_set_cursor(struct vc_struct *vc)
{
	unsigned long offset = vc->vc_pos - vga_vram_base;

	irq_disable();
	write_vga(VGA_CRTC_CURSOR_HI, 0xff&(offset >> 9));
	write_vga(VGA_CRTC_CURSOR_LO, 0xff&(offset >> 1));
	irq_enable();
}

static void vgacon_cursor(struct vc_struct *vc, int mode)
{
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

/* Clear VERTICAL area */
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
	
	delta = lines * ROW_SIZE;

	switch (dir) {
	case (VWIN_UP):
		if ((vc->vc_visible_origin - delta) <= vga_vram_base) {
			vc->vc_visible_origin = vga_vram_base;
			vc->vc_scr_end = vga_vram_base + SCREEN_SIZE;
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
			vc->vc_scr_end = vga_vram_base + SCREEN_SIZE;
			
			/* Draw cursor */
			vga_set_cursor(vc);
		}
		break;
	};
	
	vga_set_mem_top(vc);
}

/*
 * Much of the dirty work seems should be done in boot process. For simplicity,
 * we assume that in mode 3, an EGA or a VGA+ is present. So the vram_size is
 * 0x8000. How to detect the type of video card? Please see linux/boot/video
 * for details.
 */
static void vgacon_startup(void)
{
	/* boot_params.screen_info initialized? */
	if ((screen_info.orig_video_mode  == 0) &&
	    (screen_info.orig_video_lines == 0) &&
	    (screen_info.orig_video_cols  == 0))
	    	return;

	switch(screen_info.orig_video_mode) {
	case 3:
		/* Color Display. EGA, VGA+ */
		vga_vram_base = 0xb8000;
		vga_vram_size = 0x8000;
		vga_video_port_reg = VGA_CRT_IC;
		vga_video_port_val = VGA_CRT_DC;
		break;
	case 7:
		/* Monochrome Display. MDA */
		vga_vram_base = 0xb0000;
		vga_vram_size = 0x2000;
		vga_video_port_reg = VGA_CRT_IM;
		vga_video_port_val = VGA_CRT_DM;
		break;
	default:
		return;
	}

	vga_vram_attr = 0x7;
	vga_erased_char = (vga_vram_attr << 8) | BLANK;
	
	vga_vram_base = VGA_MEM_MAP(vga_vram_base);
	vga_vram_end = vga_vram_base + vga_vram_size;
	vga_visible_origin = vga_vram_base;

	vga_video_num_cols = screen_info.orig_video_cols;
	vga_video_num_rows = screen_info.orig_video_lines;

	vga_x = screen_info.orig_x;
	vga_y = screen_info.orig_y;
	vga_pos = VGA_OFFSET(vga_y, vga_x) + vga_vram_base;
}

/* 
 * This functions is called everytime VC starts a new console. It is used to
 * initialize the new vc_struct. Note that, if it is the first time to call
 * vgacon_init, vgacon_startup must be called first. And also, in real system,
 * a big memory area should be allocated for this new virtual console.
 */
static void vgacon_init(struct vc_struct *vc)
{
	vc->vc_x	= vga_x;
	vc->vc_y	= vga_y;
	vc->vc_pos	= vga_pos;
	vc->vc_cols	= vga_video_num_cols;
	vc->vc_rows	= vga_video_num_rows;
	vc->vc_row_size	= vga_video_num_cols << 1;
	vc->vc_scr_end	= vga_visible_origin + SCREEN_SIZE;
	vc->vc_attr	= vga_vram_attr;
	vc->vc_origin	= vga_vram_base;
	vc->vc_visible_origin = vga_visible_origin;
	vc->vc_erased_char = vga_erased_char;
}

static void vgacon_deinit(struct vc_struct *vc)
{
	return;
}

/*
 * 00H black
 * 01H blue
 * 02H green
 * 03H cyan
 * 04H red
 * 05H magenta
 * 06H brown
 * 07H white
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
