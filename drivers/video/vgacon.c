/*
 * Low Level VGA-Based Console Driver.
 * 
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive for
 * more details.
 */

#include <sandix/bootparam.h>
#include <sandix/irq.h>
#include <sandix/screen_info.h>
#include <sandix/types.h>
#include <video/vga.h>

#define SCREEN_START 0xB8000
#define SCREEN_END   0xC0000
#define ATTRIBUTE    0x07
#define LINES	25
#define COLUMNS	80
#define NPAR	16

static u16	vga_video_port_reg;	/* Video register select port */
static u16	vga_video_port_val;	/* Video register value port */
static u32	vga_visible_origin;	/* Upper left character */
static u32	vga_vram_base;		/* Base of video memory */
static u32	vga_vram_end;		/* End of video memory */
static u32	vga_vram_size;		/* Size of video memory */
static u32	vga_pos;		/* Cursor position addr */
static u32	vga_x;
static u32	vga_y;
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

static inline void vga_set_cursor(void)
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
	}
	else {
		/* Color Display*/
		vga_vram_base = 0xb8000;
		vga_video_port_reg = VGA_CRT_IC;
		vga_video_port_val = VGA_CRT_DC;
	}
}







