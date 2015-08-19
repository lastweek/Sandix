/*
 *	include/sandix/screen_info.h - Boot Screen Info
 *
 *	Copyright (C) 2015 Yizhou Shan <shanyizhou@ict.ac.cn>
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

#ifndef _SANDIX_SCREEN_INFO_H_
#define _SANDIX_SCREEN_INFO_H_

#define VIDEO_TYPE_MDA		0x10	/* Monochrome Text Display	*/
#define VIDEO_TYPE_CGA		0x11	/* CGA Display 			*/
#define VIDEO_TYPE_EGAM		0x20	/* EGA/VGA in Monochrome Mode	*/
#define VIDEO_TYPE_EGAC		0x21	/* EGA in Color Mode		*/
#define VIDEO_TYPE_VGAC		0x22	/* VGA+ in Color Mode		*/
#define VIDEO_TYPE_VLFB		0x23	/* VESA VGA in graphic mode	*/

/*
 * Screen info fetched from BIOS intcall and BIOS Data Area.
 * The setup routine is boot/video.c Size = 12 bytes
 */
struct screen_info {
	unsigned char	orig_x;
	unsigned char	orig_y;
	unsigned short	orig_video_page;
	
	unsigned char 	orig_video_mode;
	unsigned char 	orig_video_cols;
	unsigned char	orig_video_lines;
	unsigned char	orig_video_isVGA;
	
	unsigned short	orig_video_points;
	unsigned char	pad[2];
} __attribute__((packed));

/* Defined in init/main.c */
extern struct screen_info screen_info;

#endif /* _SANDIX_SCREEN_INFO_H_ */
