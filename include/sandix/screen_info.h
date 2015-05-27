#ifndef _SANDIX_SCREEN_INFO_H_
#define _SANDIX_SCREEN_INFO_H_

#include <sandix/types.h>

#define VIDEO_TYPE_MDA		0x10	/* Monochrome Text Display	*/
#define VIDEO_TYPE_CGA		0x11	/* CGA Display 			*/
#define VIDEO_TYPE_EGAM		0x20	/* EGA/VGA in Monochrome Mode	*/
#define VIDEO_TYPE_EGAC		0x21	/* EGA in Color Mode		*/
#define VIDEO_TYPE_VGAC		0x22	/* VGA+ in Color Mode		*/
#define VIDEO_TYPE_VLFB		0x23	/* VESA VGA in graphic mode	*/

/**
 * Screen info fetched from BIOS intcall and
 * BIOS Data Area. The setup routine is boot/video.c
 **/
struct screen_info {
	__u8	orig_x;
	__u8	orig_y;
	__u16	orig_video_page;
	
	__u8 	orig_video_mode;
	__u8 	orig_video_cols;
	__u8	orig_video_lines;
	__u8	orig_video_isVGA;
	
	__u16	orig_video_points;
	__u8	pad[2];
} __attribute__((packed));

#endif /* _SANDIX_SCREEN_INFO_H_ */
