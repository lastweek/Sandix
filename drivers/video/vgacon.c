/*
 * Low-level VGA-based Console Driver.
 *
 * Including some common operations that can be used
 * by VC layer. This is a very simple implementation.
 * And it is the upper layer, VC, who responsible for
 * emulate VT102 and processing the escape sequence.
 *
 * Tue Jul 28 13:25:24 CST 2015
 */

#include <sandix/bootparam.h>
#include <sandix/console.h>
#include <sandix/irq.h>
#include <sandix/screen_info.h>
#include <sandix/types.h>
#include <video/vga.h>
#include <video/video.h>

static	u16	vga_video_port_reg;	/* Video register select port */
static	u16	vga_video_port_val;	/* Video register value port */
static	u32	vga_visible_origin;	/* Upper left character */
static	u32	vga_vram_base;		/* Base of video memory */
static	u32	vga_vram_end;		/* End of video memory */
static	u32	vga_vram_size;		/* Size of video memory */
static	u32	vga_pos;		/* Cursor position address */
static	u32	vga_x;			/* Cursor x */
static	u32	vga_y;			/* Cursor y */
static	u32	vga_video_num_columns;	/* Number of text columns */
static	u32	vga_video_num_lines;	/* Number of text lines */

static inline void write_vga(unsigned char reg, unsigned char val)
{
	outb(reg, vga_video_port_reg);
	outb(val, vga_video_port_val);
}

static inline void vga_set_mem_top(void)
{
	u32 offset;
	
	irq_disable();
	offset = (vga_visible_origin - vga_vram_base) / 2;
	write_vga(VGA_CRTC_START_HI, (u8)(offset >> 8));
	write_vga(VGA_CRTC_START_LO, (u8)(offset));
	irq_enable();
}

static inline void vga_cursor(struct vc_struct *v)
{
	u32 offset;

	irq_disable();
	offset = (vga_pos - vga_vram_base) / 2;
	write_vga(VGA_CRTC_CURSOR_HI, (u8)(offset >> 8));
	write_vga(VGA_CRTC_CURSOR_LO, (u8)(offset));
	irq_enable();
}

static void vgacon_startup(void)
{
	if (screen_info.orig_video_mode == 7) {
		/* Monochrome Display */
		vga_vram_base = 0xb0000;
		vga_video_port_reg = VGA_CRT_IM;
		vga_video_port_val = VGA_CRT_DM;
	} else {
		/* Color Display */
		/* We always reach here, since we use Mode 3 only */
		/* See boot/video.c for details */
		vga_vram_base = 0xb8000;
		vga_video_port_reg = VGA_CRT_IC;
		vga_video_port_val = VGA_CRT_DC;
	}
}

static int vgacon_putc(struct vc_strcut *vc)
{

}

static int vgacon_putcs(struct vc_struct *vc)
{

}

static int vgacon_scroll(struct vc_struct *vc)
{
	
}

const struct con_driver vga_con = {
	.con_startup	=	vgacon_startup;
	.con_init	=	vgacon_init;
	.con_deinit	=	vgacon_deinit;
	.con_clear	=	vgacon_clear;
	.con_putc	=	vgacon_putc;
	.con_putcs	=	vgacon_putcs;
	.con_cursor	=	vgacon_cursor;
	.con_scroll	=	vgacon_scroll;
};
