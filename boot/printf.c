/*
 *	2014/04/18 Created by Shan Yizhou.
 *
 *	printf.c: Fotmat Output for Real-Mode debug.
 *	No qualifier, No alignment. Minimum supported
 *	modifiers: %c %s %d %u %p %x %X %o
 */

#include <stdarg.h>
#include <types.h>
#include "boot.h"

int vsprintf(char *buf, const char *fmt, va_list args)
{
	char *str;

	for (str = buf; *fmt; fmt++) {
		if (*fmt != '%') {
			*str++ = *fmt;
			continue;
		}

		++fmt; /* skip '%' */
		switch (*fmt) {
			case 'c':
			
			case 's':

			case 'd':

			case 'u':

			case 'p':
			case 'x':
			case 'X':
			case 'o':
			default:
		}/* EndOfSwitch */ 

	}/* EnfOfFor */
	
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

	puts(buf);

	return printed;
}

