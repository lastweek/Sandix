/*
 *	Copyright (C) 2015-2016 Yizhou Shan <shan13@purdue.edu>
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

#ifndef _VIDEO_VIDEO_H_
#define _VIDEO_VIDEO_H_

/*
 * This is a set of macros and functions which are used in the console
 * drivers. In most cases the console works with simple in-memory buffer,
 * but when handling hardware text mode consoles, the foreground console
 * is manipulated through video memory directly.
 */

#include <sandix/types.h>
#include <sandix/compiler.h>

#define scr_writeb(value, addr)	(*(u8 *)(addr) = (u8)(value))
#define scr_writew(value, addr)	(*(u16 *)(addr) = (u16)(value))
#define scr_readb(addr)		(*(u8 *)(addr))
#define scr_readw(addr)		(*(u16 *)(addr))

static __always_inline void scr_memsetw(u16 *d, u16 v, unsigned int count)
{
	while (count--) {
		scr_writew(v, d++);
	}
}

static __always_inline void scr_memcpyw(u16 *d, u16 *s, unsigned int count)
{
	while (count--) {
		scr_writew(scr_readw(s++), d++);
	}
}

static __always_inline void scr_memmovew(u16 *d, u16 *s, unsigned int count)
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
