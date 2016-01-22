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
 * Simple printf implementation
 */

#include "boot.h"
#include <stdarg.h>
#include <sandix/types.h>

#define __SIGN__		1
#define __UNSIGN__		2
#define __UHEX__		3
#define __LHEX__		4
#define MAX_LEN 		10

static char *number(char *str, int base, int num, int type)
{
	int remainder, len;
	char reverse[MAX_LEN] = {'\0'};
	char *digits;
	char ASCII_NUMS[16] = "0123456789ABCDEF";
	char ascii_nums[16] = "0123456789abcdef";

	if (type == __UHEX__)
		digits = ASCII_NUMS;
	else
		digits = ascii_nums;
	
	//digits = (type == __UHEX__)? ASCII_NUMS : ascii_nums;

	if (base == 10 && type == __SIGN__) {
		if (num & 0x80000000) {
			*str++ = '-';
			num = ~num + 1;
		}
	}

	if (!num) {
		*str++ = '0';
		return str;
	}

	for (len = 0; ; len++) {
		remainder = (u32)num % (u32)base;
		num = (u32)num / (u32)base;
		if (!num && !remainder)
			break;
		reverse[len] = digits[remainder];
	}
	
	while (len) {
		*str++ = reverse[len-1];
		len--;
	}

	return str;
}

int vsprintf(char *buf, const char *fmt, va_list args)
{
	int  sint;
	u8   ch;
	u32  uint;
	char *s;
	char *str;

	for (str = buf; *fmt; fmt++) {
		if (*fmt != '%') {
			*str++ = *fmt;
			continue;
		}

		++fmt; /* skip '%' */
		switch (*fmt) {
			case 'c':
				ch = va_arg(args, u8);
				*str++ = ch;
				continue;
			case 's':
				/* FIXME Handle printf("%s", 'A'); */
				/* That is not SUPPORTED even in GLIBC */
				s = va_arg(args, char *);
				while (*s) /* if str overfill? */
					*str++ = *s++;
				continue;
			case 'd':
				sint = va_arg(args, int);
				str = number(str, 10, sint, __SIGN__);
				continue;
			/* Maybe later.TODO
			case 'l':
				if ((*(fmt+1)) == 'd') {
					fmt += 2;
					lint = va_arg(args, long long);	
					str = number(str, 10, lint, __SIGN__);
				}
			*/
			case 'u':
				uint = va_arg(args, u32);
				str = number(str, 10, uint, __UNSIGN__);
				continue;
			case 'o':
				uint = va_arg(args, u32);
				str = number(str, 8, uint, __UNSIGN__);
				continue;
			case 'p':
				uint = va_arg(args, u32);
				*str++ = '0';
				*str++ = 'x';
				str = number(str, 16, uint, __LHEX__);
				continue;
			case 'X':
				uint = va_arg(args, u32);
				*str++ = '0';
				*str++ = 'x';
				str = number(str, 16, uint, __UHEX__);
				continue;
			case 'x':
				uint = va_arg(args, u32);
				*str++ = '0';
				*str++ = 'x';
				str = number(str, 16, uint, __LHEX__);
				continue;
			case 'Z':
				uint = va_arg(args, u32);
				str = number(str, 16, uint, __UHEX__);
				continue;
			case 'z':
				uint = va_arg(args, u32);
				str = number(str, 16, uint, __LHEX__);
				continue;
			case '%':
				*str++ = '%';
				continue;
			default:/* Undefined */
				return -1;
		}/* EndOfSwitch */ 
	}/* EnfOfFor */
	
	*str++ = '\0';
	return (str - buf);
}

int sprintf(char *buf, const char *fmt, ...)
{
	va_list args;
	int printed;

	va_start(args, fmt);
	printed = vsprintf(buf, fmt, args);
	va_end(args);

	return printed;
}

int printf(const char *fmt, ...)
{
	char buf[1024];
	va_list args;
	int printed;

	va_start(args, fmt);
	printed = vsprintf(buf, fmt, args);
	va_end(args);
	
	if (printed != -1)
		puts(buf);

	return printed;
}
