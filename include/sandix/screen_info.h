#ifndef _SANDIX_SCREEN_INFO_H_
#define _SANDIX_SCREEN_INFO_H_

#define VIDEO_TYPE_MDA		0x10	/* Monochrome Text Display	*/
#define VIDEO_TYPE_CGA		0x11	/* CGA Display 			*/
#define VIDEO_TYPE_EGAM		0x20	/* EGA/VGA in Monochrome Mode	*/
#define VIDEO_TYPE_EGAC		0x21	/* EGA in Color Mode		*/
#define VIDEO_TYPE_VGAC		0x22	/* VGA+ in Color Mode		*/
#define VIDEO_TYPE_VLFB		0x23	/* VESA VGA in graphic mode	*/

/**
 * Screen info fetched from BIOS intcall and
 * BIOS Data Area. The setup routine is boot/video.c
 * Size = 12 bytes
 **/
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

#endif /* _SANDIX_SCREEN_INFO_H_ */
