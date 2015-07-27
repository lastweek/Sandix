#ifndef _VIDEO_VIDEO_H_
#define _VIDEO_VIDEO_H_

#define scr_writeb(value, addr) (*(char *)(addr) = (char)(value))
#define scr_readb(addr) (*(char *)(addr))

#endif /* _VIDEO_VIDEO_H_ */
