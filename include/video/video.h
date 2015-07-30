/*
 * Common screen buffer access functions.
 *
 * Copyright (C) 2015	Yizhou Shan
 *
 * This is a set of macros and functions which are used in the console
 * drivers. In most cases the console works with simple in-memory buffer,
 * but when handling hardware text mode consoles, the foreground console
 * is manipulated through video memory directly.
 */
 
#ifndef _VIDEO_VIDEO_H_
#define _VIDEO_VIDEO_H_

#include <sandix/compiler.h>
#include <sandix/types.h>

#define scr_writeb(value, addr)	(*(u8 *)(addr) = (u8)(value))
#define scr_writew(value, addr)	(*(u16 *)(addr) = (u16)(value))
#define scr_readb(addr)		(*(u8 *)(addr))
#define scr_readw(addr)		(*(u16 *)(addr))

ALWAYS_INLINE void scr_memsetw(u16 *d, u16 v, unsigned int count)
{
	while (count--) {
		scr_writew(v, d++);
	}
}

ALWAYS_INLINE void scr_memcpyw(u16 *d, u16 *s, unsigned int count)
{
	while (count--) {
		scr_writew(scr_readw(s++), d++);
	}
}

ALWAYS_INLINE void scr_memmovew(u16 *d, u16 *s, unsigned int count)
{
	if (d < s) {
		scr_memcpyw(d, s, count);
	} else {
		d += count*2;
		s += count*2;
		while (count--)
			scr_writew(scr_readw(--s), --d);
	}
}

#endif /* _VIDEO_VIDEO_H_ */
