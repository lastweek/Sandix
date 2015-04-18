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
				s = va_arg(args, char *);
				while (*s) /* if str overfill? */
					*str++ = *s++;
				continue;
			case 'd':
				sint = va_arg(args, int);
				continue;
			case 'u':
				uint = va_arg(args, u32);
				continue;
			case 'p':
				uint = va_arg(args, u32);
				continue;
			case 'x':
				uint = va_arg(args, u32);
				continue;
			case 'X':
				uint = va_arg(args, u32);
				continue;
			case 'o':
				uint = va_arg(args, u32);
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

