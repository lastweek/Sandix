#ifndef _VIDEO_VIDEO_H_
#define _VIDEO_VIDEO_H_

#include <sandix/types.h>

#define scr_writeb(value, addr)	(*(u8 *)(addr) = (u8)(value))
#define scr_readb(addr)		(*(u8 *)(addr))

#define scr_writew(value, addr)	(*(u16 *)(addr) = (u16)(value))
#define scr_readw(addr)		(*(u16 *)(addr))

static inline void scr_memsetw(u16 *d, u16 v, unsigned int count)
{
	while (count--) {
		scr_writew(v, d++);
	}
}

#endif /* _VIDEO_VIDEO_H_ */
