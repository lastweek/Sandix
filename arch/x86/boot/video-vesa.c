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

/*
 * VESA text modes
 */

#include "boot.h"
#include "video.h"
#include "vesa.h"
#include "string.h"

#include <asm/bootparam.h>

/* VESA information */
static struct vesa_general_info vginfo;
static struct vesa_mode_info vminfo;

static __videocard video_vesa;

#ifndef _WAKEUP
static void vesa_store_mode_params_graphics(void);
#else
static inline void vesa_store_mode_params_graphics(void) {}
#endif

/* Ralf Brown Interrupt List
--------V-104F00-----------------------------
INT 10 - VESA SuperVGA BIOS (VBE) - GET SuperVGA INFORMATION
	AX = 4F00h
	ES:DI -> buffer for SuperVGA information (see #00077)
Return: AL = 4Fh if function supported
	AH = status
	    00h successful
		ES:DI buffer filled
	    01h failed
	    ---VBE v2.0---
	    02h function not supported by current hardware configuration
	    03h function invalid in current video mode
Desc:	determine whether VESA BIOS extensions are present and the capabilities
	  supported by the display adapter
SeeAlso: AX=4E00h,AX=4F01h,AX=7F00h"SOLLEX",AX=A00Ch
Index:	installation check;VESA SuperVGA

Format of SuperVGA information:
Offset	Size	Description	(Table 00077)
 00h  4 BYTEs	(ret) signature ("VESA")
		(call) VESA 2.0 request signature ("VBE2"), required to receive
		  version 2.0 info
 04h	WORD	VESA version number (one-digit minor version -- 0102h = v1.2)
 06h	DWORD	pointer to OEM name
		"761295520" for ATI
 0Ah	DWORD	capabilities flags (see #00078)
 0Eh	DWORD	pointer to list of supported VESA and OEM video modes
		(list of words terminated with FFFFh)
 12h	WORD	total amount of video memory in 64K blocks
---VBE v1.x ---
 14h 236 BYTEs	reserved
---VBE v2.0 ---
 14h	WORD	OEM software version (BCD, high byte = major, low byte = minor)
 16h	DWORD	pointer to vendor name
 1Ah	DWORD	pointer to product name
 1Eh	DWORD	pointer to product revision string
 22h	WORD	(if capabilities bit 3 set) VBE/AF version (BCD)
		0100h for v1.0P
 24h	DWORD	(if capabilities bit 3 set) pointer to list of supported
		  accelerated video modes (list of words terminated with FFFFh)
 28h 216 BYTEs	reserved for VBE implementation
100h 256 BYTEs	OEM scratchpad (for OEM strings, etc.)
Notes:	the list of supported video modes is stored in the reserved portion of
	  the SuperVGA information record by some implementations, and it may
	  thus be necessary to either copy the mode list or use a different
	  buffer for all subsequent VESA calls
	not all of the video modes in the list of mode numbers may be
	  supported, e.g. if they require more memory than currently installed
	  or are not supported by the attached monitor.	 Check any mode you
	  intend to use through AX=4F01h first.
	the 1.1 VESA document specifies 242 reserved bytes at the end, so the
	  buffer should be 262 bytes to ensure that it is not overrun; for
	  v2.0, the buffer should be 512 bytes
	the S3 specific video modes will most likely follow the FFFFh
	  terminator at the end of the standard modes.	A search must then
	  be made to find them, FFFFh will also terminate this second list
	in some cases, only a "stub" VBE may be present, supporting only
	  AX=4F00h; this case may be assumed if the list of supported video
	  modes is empty (consisting of a single word of FFFFh)

Bitfields for VESA capabilities:
Bit(s)	Description	(Table 00078)
 0	DAC can be switched into 8-bit mode
 1	non-VGA controller
 2	programmed DAC with blank bit (i.e. only during blanking interval)
 3	(VBE v3.0) controller supports hardware stereoscopic signalling
 3	controller supports VBE/AF v1.0P extensions
 4	(VBE v3.0) if bit 3 set:
	    =0 stereo signalling via external VESA stereo connector
	    =1 stereo signalling via VESA EVC connector
 4	(VBE/AF v1.0P) must call EnableDirectAccess to access framebuffer
 5	(VBE/AF v1.0P) controller supports hardware mouse cursor
 6	(VBE/AF v1.0P) controller supports hardware clipping
 7	(VBE/AF v1.0P) controller supports transparent BitBLT
 8-31	reserved (0)
SeeAlso: #00077,AX=4F09h
--------V-104F01-----------------------------
INT 10 - VESA SuperVGA BIOS - GET SuperVGA MODE INFORMATION
	AX = 4F01h
	CX = SuperVGA video mode (see #04082 for bitfields)
	ES:DI -> 256-byte buffer for mode information (see #00079)
Return: AL = 4Fh if function supported
	AH = status
	    00h successful
		ES:DI buffer filled
	    01h failed
Desc:	determine the attributes of the specified video mode
SeeAlso: AX=4F00h,AX=4F02h

Bitfields for VESA/VBE video mode number:
Bit(s)	Description	(Table 04082)
 15	preserve display memory on mode change
 14	(VBE v2.0+) use linear (flat) frame buffer
 13	(VBE/AF 1.0P) VBE/AF initializes accelerator hardware
 12	reserved for VBE/AF
 11	(VBE v3.0) user user-specified CRTC refresh rate values
 10-9	reserved for future expansion
 8-0	video mode number (0xxh are non-VESA modes, 1xxh are VESA-defined)

Format of VESA SuperVGA mode information:
Offset	Size	Description	(Table 00079)
 00h	WORD	mode attributes (see #00080)
 02h	BYTE	window attributes, window A (see #00081)
 03h	BYTE	window attributes, window B (see #00081)
 04h	WORD	window granularity in KB
 06h	WORD	window size in KB
 08h	WORD	start segment of window A (0000h if not supported)
 0Ah	WORD	start segment of window B (0000h if not supported)
 0Ch	DWORD	-> FAR window positioning function (equivalent to AX=4F05h)
 10h	WORD	bytes per scan line
---remainder is optional for VESA modes in v1.0/1.1, needed for OEM modes---
 12h	WORD	width in pixels (graphics) or characters (text)
 14h	WORD	height in pixels (graphics) or characters (text)
 16h	BYTE	width of character cell in pixels
 17h	BYTE	height of character cell in pixels
 18h	BYTE	number of memory planes
 19h	BYTE	number of bits per pixel
 1Ah	BYTE	number of banks
 1Bh	BYTE	memory model type (see #00082)
 1Ch	BYTE	size of bank in KB
 1Dh	BYTE	number of image pages (less one) that will fit in video RAM
 1Eh	BYTE	reserved (00h for VBE 1.0-2.0, 01h for VBE 3.0)
---VBE v1.2+ ---
 1Fh	BYTE	red mask size
 20h	BYTE	red field position
 21h	BYTE	green mask size
 22h	BYTE	green field size
 23h	BYTE	blue mask size
 24h	BYTE	blue field size
 25h	BYTE	reserved mask size
 26h	BYTE	reserved mask position
 27h	BYTE	direct color mode info
		bit 0: color ramp is programmable
		bit 1: bytes in reserved field may be used by application
---VBE v2.0+ ---
 28h	DWORD	physical address of linear video buffer
 2Ch	DWORD	pointer to start of offscreen memory
 30h	WORD	KB of offscreen memory
---VBE v3.0 ---
 32h	WORD	bytes per scan line in linear modes
 34h	BYTE	number of images (less one) for banked video modes
 35h	BYTE	number of images (less one) for linear video modes
 36h	BYTE	linear modes: size of direct color red mask (in bits)
 37h	BYTE	linear modes: bit position of red mask LSB (e.g. shift count)
 38h	BYTE	linear modes: size of direct color green mask (in bits)
 39h	BYTE	linear modes: bit position of green mask LSB (e.g. shift count)
 3Ah	BYTE	linear modes: size of direct color blue mask (in bits)
 3Bh	BYTE	linear modes: bit position of blue mask LSB (e.g. shift count)
 3Ch	BYTE	linear modes: size of direct color reserved mask (in bits)
 3Dh	BYTE	linear modes: bit position of reserved mask LSB
 3Eh	DWORD	maximum pixel clock for graphics video mode, in Hz
 42h 190 BYTEs	reserved (0)
Note:	while VBE 1.1 and higher will zero out all unused bytes of the buffer,
	  v1.0 did not, so applications that want to be backward compatible
	  should clear the buffer before calling

Bitfields for VESA SuperVGA mode attributes:
Bit(s)	Description	(Table 00080)
 0	mode supported by present hardware configuration
 1	optional information available (must be =1 for VBE v1.2+)
 2	BIOS output supported
 3	set if color, clear if monochrome
 4	set if graphics mode, clear if text mode
---VBE v2.0+ ---
 5	mode is not VGA-compatible
 6	bank-switched mode not supported
 7	linear framebuffer mode supported
 8	double-scan mode available (e.g. 320x200 and 320x240)
---VBE v3.0 ---
 9	interlaced mode available
 10	hardware supports triple buffering
 11	hardware supports stereoscopic display
 12	dual display start address support
 13-15	reserved
---VBE/AF v1.0P---
 9	application must call EnableDirectAccess before calling bank-switching
	  functions
SeeAlso: #00079

Bitfields for VESA SuperVGA window attributes:
Bit(s)	Description	(Table 00081)
 0	exists
 1	readable
 2	writable
 3-7	reserved
SeeAlso: #00079

(Table 00082)
Values for VESA SuperVGA memory model type:
 00h	text
 01h	CGA graphics
 02h	HGC graphics
 03h	16-color (EGA) graphics
 04h	packed pixel graphics
 05h	"sequ 256" (non-chain 4) graphics
 06h	direct color (HiColor, 24-bit color)
 07h	YUV (luminance-chrominance, also called YIQ)
 08h-0Fh reserved for VESA
 10h-FFh OEM memory models
SeeAlso: #00079
*/

static int vesa_probe(void)
{
	struct biosregs ireg, oreg;
	u16 mode;
	addr_t mode_ptr;
	struct mode_info *mi;
	int nmodes = 0;

	video_vesa.modes = GET_HEAP(struct mode_info, 0);

	initregs(&ireg);
	ireg.ax = 0x4f00;
	ireg.di = (size_t)&vginfo;
	intcall(0x10, &ireg, &oreg);

	if (oreg.ax != 0x004f ||
	    vginfo.signature != VESA_MAGIC ||
	    vginfo.version < 0x0102)
		return 0;	/* Not present */

	set_fs(vginfo.video_mode_ptr.seg);
	mode_ptr = vginfo.video_mode_ptr.off;

	while ((mode = rdfs16(mode_ptr)) != 0xffff) {
		mode_ptr += 2;

		if (!heap_free(sizeof(struct mode_info)))
			break;	/* Heap full, can't save mode info */

		if (mode & ~0x1ff)
			continue;

		memset(&vminfo, 0, sizeof vminfo); /* Just in case... */

		ireg.ax = 0x4f01;
		ireg.cx = mode;
		ireg.di = (size_t)&vminfo;
		intcall(0x10, &ireg, &oreg);

		if (oreg.ax != 0x004f)
			continue;

		if ((vminfo.mode_attr & 0x15) == 0x05) {
			/* Text Mode, TTY BIOS supported,
			   supported by hardware */
			mi = GET_HEAP(struct mode_info, 1);
			mi->mode  = mode + VIDEO_FIRST_VESA;
			mi->depth = 0; /* text */
			mi->x     = vminfo.h_res;
			mi->y     = vminfo.v_res;
			nmodes++;
		} else if ((vminfo.mode_attr & 0x99) == 0x99 &&
			   (vminfo.memory_layout == 4 ||
			    vminfo.memory_layout == 6) &&
			   vminfo.memory_planes == 1) {
#ifdef CONFIG_FB_BOOT_VESA_SUPPORT
			/* Graphics mode, color, linear frame buffer
			   supported.  Only register the mode if
			   if framebuffer is configured, however,
			   otherwise the user will be left without a screen. */
			mi = GET_HEAP(struct mode_info, 1);
			mi->mode = mode + VIDEO_FIRST_VESA;
			mi->depth = vminfo.bpp;
			mi->x = vminfo.h_res;
			mi->y = vminfo.v_res;
			nmodes++;
#endif
		}
	}

	return nmodes;
}

static int vesa_set_mode(struct mode_info *mode)
{
	struct biosregs ireg, oreg;
	int is_graphic;
	u16 vesa_mode = mode->mode - VIDEO_FIRST_VESA;

	memset(&vminfo, 0, sizeof vminfo); /* Just in case... */

	initregs(&ireg);
	ireg.ax = 0x4f01;
	ireg.cx = vesa_mode;
	ireg.di = (size_t)&vminfo;
	intcall(0x10, &ireg, &oreg);

	if (oreg.ax != 0x004f)
		return -1;

	if ((vminfo.mode_attr & 0x15) == 0x05) {
		/* It's a supported text mode */
		is_graphic = 0;
#ifdef CONFIG_FB_BOOT_VESA_SUPPORT
	} else if ((vminfo.mode_attr & 0x99) == 0x99) {
		/* It's a graphics mode with linear frame buffer */
		is_graphic = 1;
		vesa_mode |= 0x4000; /* Request linear frame buffer */
#endif
	} else {
		return -1;	/* Invalid mode */
	}


	initregs(&ireg);
	ireg.ax = 0x4f02;
	ireg.bx = vesa_mode;
	intcall(0x10, &ireg, &oreg);

	if (oreg.ax != 0x004f)
		return -1;

	graphic_mode = is_graphic;
	if (!is_graphic) {
		/* Text mode */
		force_x = mode->x;
		force_y = mode->y;
		do_restore = 1;
	} else {
		/* Graphics mode */
		vesa_store_mode_params_graphics();
	}

	return 0;
}


#ifndef _WAKEUP

/* Switch DAC to 8-bit mode */
static void vesa_dac_set_8bits(void)
{
	struct biosregs ireg, oreg;
	u8 dac_size = 6;

	/* If possible, switch the DAC to 8-bit mode */
	if (vginfo.capabilities & 1) {
		initregs(&ireg);
		ireg.ax = 0x4f08;
		ireg.bh = 0x08;
		intcall(0x10, &ireg, &oreg);
		if (oreg.ax == 0x004f)
			dac_size = oreg.bh;
	}

	/* Set the color sizes to the DAC size, and offsets to 0 */
	boot_params.screen_info.red_size   = dac_size;
	boot_params.screen_info.green_size = dac_size;
	boot_params.screen_info.blue_size  = dac_size;
	boot_params.screen_info.rsvd_size  = dac_size;

	boot_params.screen_info.red_pos    = 0;
	boot_params.screen_info.green_pos  = 0;
	boot_params.screen_info.blue_pos   = 0;
	boot_params.screen_info.rsvd_pos   = 0;
}

/* Save the VESA protected mode info */
static void vesa_store_pm_info(void)
{
	struct biosregs ireg, oreg;

	initregs(&ireg);
	ireg.ax = 0x4f0a;
	intcall(0x10, &ireg, &oreg);

	if (oreg.ax != 0x004f)
		return;

	boot_params.screen_info.vesapm_seg = oreg.es;
	boot_params.screen_info.vesapm_off = oreg.di;
}

/*
 * Save video mode parameters for graphics mode
 */
static void vesa_store_mode_params_graphics(void)
{
	/* Tell the kernel we're in VESA graphics mode */
	boot_params.screen_info.orig_video_isVGA = VIDEO_TYPE_VLFB;

	/* Mode parameters */
	boot_params.screen_info.vesa_attributes = vminfo.mode_attr;
	boot_params.screen_info.lfb_linelength = vminfo.logical_scan;
	boot_params.screen_info.lfb_width = vminfo.h_res;
	boot_params.screen_info.lfb_height = vminfo.v_res;
	boot_params.screen_info.lfb_depth = vminfo.bpp;
	boot_params.screen_info.pages = vminfo.image_planes;
	boot_params.screen_info.lfb_base = vminfo.lfb_ptr;
	memcpy(&boot_params.screen_info.red_size,
	       &vminfo.rmask, 8);

	/* General parameters */
	boot_params.screen_info.lfb_size = vginfo.total_memory;

	if (vminfo.bpp <= 8)
		vesa_dac_set_8bits();

	vesa_store_pm_info();
}

/*
 * Save EDID information for the kernel; this is invoked, separately,
 * after mode-setting.
 */
void vesa_store_edid(void)
{
#ifdef CONFIG_FIRMWARE_EDID
	struct biosregs ireg, oreg;

	/* Apparently used as a nonsense token... */
	memset(&boot_params.edid_info, 0x13, sizeof boot_params.edid_info);

	if (vginfo.version < 0x0200)
		return;		/* EDID requires VBE 2.0+ */

	initregs(&ireg);
	ireg.ax = 0x4f15;		/* VBE DDC */
	/* ireg.bx = 0x0000; */		/* Report DDC capabilities */
	/* ireg.cx = 0;	*/		/* Controller 0 */
	ireg.es = 0;			/* ES:DI must be 0 by spec */
	intcall(0x10, &ireg, &oreg);

	if (oreg.ax != 0x004f)
		return;		/* No EDID */

	/* BH = time in seconds to transfer EDD information */
	/* BL = DDC level supported */

	ireg.ax = 0x4f15;		/* VBE DDC */
	ireg.bx = 0x0001;		/* Read EDID */
	/* ireg.cx = 0; */		/* Controller 0 */
	/* ireg.dx = 0;	*/		/* EDID block number */
	ireg.es = ds();
	ireg.di =(size_t)&boot_params.edid_info; /* (ES:)Pointer to block */
	intcall(0x10, &ireg, &oreg);
#endif /* CONFIG_FIRMWARE_EDID */
}

#endif /* not _WAKEUP */

static __videocard video_vesa =
{
	.card_name	= "VESA",
	.probe		= vesa_probe,
	.set_mode	= vesa_set_mode,
	.xmode_first	= VIDEO_FIRST_VESA,
	.xmode_n	= 0x200,
};
