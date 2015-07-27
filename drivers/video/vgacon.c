/*
 * Low-level VGA-based Console Driver.
 *
 * Including some common operations can be used
 * by VC layer. Note that this is a very simple
 * implementation.
 */

#include <sandix/bootparam.h>
#include <sandix/console.h>
#include <sandix/irq.h>
#include <sandix/screen_info.h>
#include <sandix/types.h>
#include <video/vga.h>
#include <video/video.h>

#define SCREEN_END   0xC0000
#define ATTRIBUTE    0x07

static u16	vga_video_port_reg;	/* Video register select port */
static u16	vga_video_port_val;	/* Video register value port */
static u32	vga_visible_origin;	/* Upper left character */
static u32	vga_vram_base;		/* Base of video memory */
static u32	vga_vram_end;		/* End of video memory */
static u32	vga_vram_size;		/* Size of video memory */
static u32	vga_pos;		/* Cursor position address */
static u32	vga_x;			/* Cursor x */
static u32	vga_y;			/* Cursor y */
static u32	vga_video_num_columns;	/* Number of text columns */
static u32	vga_video_num_lines;	/* Number of text lines */

static inline void write_vga(unsigned char reg, unsigned char val)
{
	outb(reg, vga_video_port_reg);
	outb(val, vga_video_port_val);
}

static inline void vga_set_top_mem(void)
{
	u32 offset;
	
	irq_disable();
	offset = (vga_visible_origin - vga_vram_base) / 2;
	write_vga(VGA_CRTC_START_HI, (u8)(offset >> 8));
	write_vga(VGA_CRTC_START_LO, (u8)(offset));
	irq_enable();
}

static inline void vga_cursor(struct vc *v)
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
		/* Monochrome display */
		vga_vram_base = 0xb0000;
		vga_video_port_reg = VGA_CRT_IM;
		vga_video_port_val = VGA_CRT_DM;
	} else {
		/* Color Display*/
		vga_vram_base = 0xb8000;
		vga_video_port_reg = VGA_CRT_IC;
		vga_video_port_val = VGA_CRT_DC;
	}
}

static int vgacon_putc(struct vc *vc)
{

}

const struct consw vga_con = {
	.con_startup	=	vgacon_startup;
	.con_init	=	vgacon_init;
	.con_deinit	=	vgacon_deinit;
	.con_clear	=	vgacon_clear;
	.con_putc	=	vgacon_putc;
	.con_putcs	=	vgacon_putcs;
	.con_cursor	=	vgacon_cursor;
	.con_scroll	=	vgacon_scroll;
};
