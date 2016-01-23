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

#ifndef _X86_BOOT_VESA_H_
#define _X86_BOOT_VESA_H_

#include <sandix/types.h>

typedef struct {
	u16 off, seg;
} far_ptr;

/* VESA General Information table */
struct vesa_general_info {
	u32 signature;		/* 0 Magic number = "VESA" */
	u16 version;		/* 4 */
	far_ptr vendor_string;	/* 6 */
	u32 capabilities;	/* 10 */
	far_ptr video_mode_ptr;	/* 14 */
	u16 total_memory;	/* 18 */

	u8 reserved[236];	/* 20 */
} __attribute__ ((packed));

#define VESA_MAGIC ('V' + ('E' << 8) + ('S' << 16) + ('A' << 24))

struct vesa_mode_info {
	u16 mode_attr;		/* 0 */
	u8 win_attr[2];		/* 2 */
	u16 win_grain;		/* 4 */
	u16 win_size;		/* 6 */
	u16 win_seg[2];		/* 8 */
	far_ptr win_scheme;	/* 12 */
	u16 logical_scan;	/* 16 */

	u16 h_res;		/* 18 */
	u16 v_res;		/* 20 */
	u8 char_width;		/* 22 */
	u8 char_height;		/* 23 */
	u8 memory_planes;	/* 24 */
	u8 bpp;			/* 25 */
	u8 banks;		/* 26 */
	u8 memory_layout;	/* 27 */
	u8 bank_size;		/* 28 */
	u8 image_planes;	/* 29 */
	u8 page_function;	/* 30 */

	u8 rmask;		/* 31 */
	u8 rpos;		/* 32 */
	u8 gmask;		/* 33 */
	u8 gpos;		/* 34 */
	u8 bmask;		/* 35 */
	u8 bpos;		/* 36 */
	u8 resv_mask;		/* 37 */
	u8 resv_pos;		/* 38 */
	u8 dcm_info;		/* 39 */

	u32 lfb_ptr;		/* 40 Linear frame buffer address */
	u32 offscreen_ptr;	/* 44 Offscreen memory address */
	u16 offscreen_size;	/* 48 */

	u8 reserved[206];	/* 50 */
} __attribute__ ((packed));

#endif				/* LIB_SYS_VESA_H */
