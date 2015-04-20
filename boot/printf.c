/*
 *	2014/04/18 Created by Shan Yizhou.
 *
 *	printf.c: Fotmat Output for Real-Mode debug.
 *	No qualifier, No alignment. Minimum supported
 *	modifiers: %c %s %d %u %p %x %X %o
 *
 *	Return Value: -1 means undefined modifier.
 */

#include <stdarg.h>
#include <types.h>
#include "boot.h"

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
	int  sint, base;
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
				s = va_arg(args, char *);
				while (*s) /* if str overfill? */
					*str++ = *s++;
				continue;
			case 'd':
				sint = va_arg(args, int);
				str = number(str, 10, sint, __SIGN__);
				continue;
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

