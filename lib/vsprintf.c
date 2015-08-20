/*
 *	lib/vsprintf.c - You know what is this
 *
 *	Copyright (C) 2015 Yizhou Shan <shanyizhou@ict.ac.cn>
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

#include <stdarg.h>
#include <sandix/compiler.h>
#include <sandix/bug.h>
#include <sandix/kernel.h>
#include <sandix/string.h>
#include <sandix/types.h>

#define SIGN	1	/* unsigned/signed, must be 1 */
#define LEFT	2	/* left justified */
#define PLUS	4	/* show plus */
#define SPACE	8	/* space if plus */
#define ZEROPAD	16	/* pad with zero, must be 16 == '0' - ' ' */
#define SMALL	32	/* use lowercase in hex (must be 32 == 0x20) */
#define SPECIAL	64	/* prefix hex with "0x", octal with "0" */

enum format_type {
	FORMAT_TYPE_NONE, /* Just a string part */
	FORMAT_TYPE_WIDTH,
	FORMAT_TYPE_PRECISION,
	FORMAT_TYPE_CHAR,
	FORMAT_TYPE_STR,
	FORMAT_TYPE_PTR,
	FORMAT_TYPE_PERCENT_CHAR,
	FORMAT_TYPE_INVALID,
	FORMAT_TYPE_LONG_LONG,
	FORMAT_TYPE_ULONG,
	FORMAT_TYPE_LONG,
	FORMAT_TYPE_UBYTE,
	FORMAT_TYPE_BYTE,
	FORMAT_TYPE_USHORT,
	FORMAT_TYPE_SHORT,
	FORMAT_TYPE_UINT,
	FORMAT_TYPE_INT,
	FORMAT_TYPE_SIZE_T,
	FORMAT_TYPE_PTRDIFF
};

struct printf_spec {
	u8	type;		/* format_type enum */
	u8	flags;		/* flags to number() */
	u8	base;		/* number base, 8, 10 or 16 only */
	u8	qualifier;	/* number qualifier, one of 'hHlLtzZ' */
	s16	field_width;	/* width of output field */
	s16	precision;	/* # of digits/chars */
};

bool isdigit(int c)
{
	if (c >= '0' && c <= '9')
		return true;
	else
		return false;
}

int skip_atoi(void *p)
{

}

/*
 * Helper function to decode printf style format.
 * Each call decode a token from the format and return the
 * number of characters read (or likely the delta where it wants
 * to go on the next call).
 * The decoded token is returned through the parameters
 *
 * @fmt: the format string
 * @type of the token returned
 * @flags: various flags such as +, -, # tokens..
 * @base: base of the number (octal, hex, ...)
 * @qualifier: qualifier of a number (long, size_t, ...)
 * @field_width: overwritten width
 * @precision: precision of a number
 */
static int format_decode(const char *fmt, struct printf_spec *spec)
{
	const char *start = fmt;

	/* we finished early by reading the field width */
	if (spec->type == FORMAT_TYPE_WIDTH) {
		if (spec->field_width < 0) {
			spec->field_width = -spec->field_width;
			spec->flags |= LEFT;
		}
		spec->type = FORMAT_TYPE_NONE;
		goto precision;
	}

	/* we finished early by reading the precision */
	if (spec->type == FORMAT_TYPE_PRECISION) {
		if (spec->precision < 0)
			spec->precision = 0;

		spec->type = FORMAT_TYPE_NONE;
		goto qualifier;
	}

	/* By default */
	spec->type = FORMAT_TYPE_NONE;

	for (; *fmt ; ++fmt) {
		if (*fmt == '%')
			break;
	}

	/* Return the current non-format string */
	if (fmt != start || !*fmt)
		return fmt - start;

	/* Process flags */
	spec->flags = 0;
	while (1) {
		bool found = true;

		/* Skip % */
		++fmt;

		switch (*fmt) {
		case '-': spec->flags |= LEFT;    break;
		case '+': spec->flags |= PLUS;    break;
		case ' ': spec->flags |= SPACE;   break;
		case '#': spec->flags |= SPECIAL; break;
		case '0': spec->flags |= ZEROPAD; break;
		default:  found = false;
		}

		if (!found)
			break;
	}

	/* Get field width */
	spec->field_width = -1;
	if (isdigit(*fmt))
		spec->field_width = skip_atoi(&fmt);
	else if (*fmt == '*') {
		/* it's the next argument */
		spec->type = FORMAT_TYPE_WIDTH;
		return ++fmt - start;
	}

precision:
	/* get the precision */
	spec->precision = -1;
	if (*fmt == '.') {
		++fmt;
		if (isdigit(*fmt)) {
			spec->precision = skip_atoi(&fmt);
			if (spec->precision < 0)
				spec->precision = 0;
		} else if (*fmt == '*') {
			/* it's the next argument */
			spec->type = FORMAT_TYPE_PRECISION;
			return ++fmt - start;
		}
	}

qualifier:
	/* get the conversion qualifier */
	spec->qualifier = -1;
	if (*fmt == 'h' || _tolower(*fmt) == 'l' ||
	    _tolower(*fmt) == 'z' || *fmt == 't') {
		spec->qualifier = *fmt++;
		if (unlikely(spec->qualifier == *fmt)) {
			if (spec->qualifier == 'l') {
				spec->qualifier = 'L';
				++fmt;
			} else if (spec->qualifier == 'h') {
				spec->qualifier = 'H';
				++fmt;
			}
		}
	}

	/* default base */
	spec->base = 10;
	switch (*fmt) {
	case 'c':
		spec->type = FORMAT_TYPE_CHAR;
		return ++fmt - start;

	case 's':
		spec->type = FORMAT_TYPE_STR;
		return ++fmt - start;

	case 'p':
		spec->type = FORMAT_TYPE_PTR;
		return ++fmt - start;

	case '%':
		spec->type = FORMAT_TYPE_PERCENT_CHAR;
		return ++fmt - start;

	/* integer number formats - set up the flags and "break" */
	case 'o':
		spec->base = 8;
		break;

	case 'x':
		spec->flags |= SMALL;

	case 'X':
		spec->base = 16;
		break;

	case 'd':
	case 'i':
		spec->flags |= SIGN;
	case 'u':
		break;

	case 'n':
		/*
		 * Since %n poses a greater security risk than utility, treat
		 * it as an invalid format specifier. Warn about its use so
		 * that new instances don't get added.
		 */
		WARN_ONCE(1, "Please remove ignored %%n in '%s'\n", fmt);
		/* Fall-through */

	default:
		spec->type = FORMAT_TYPE_INVALID;
		return fmt - start;
	}

	if (spec->qualifier == 'L')
		spec->type = FORMAT_TYPE_LONG_LONG;
	else if (spec->qualifier == 'l') {
		BUILD_BUG_ON(FORMAT_TYPE_ULONG + SIGN != FORMAT_TYPE_LONG);
		spec->type = FORMAT_TYPE_ULONG + (spec->flags & SIGN);
	} else if (_tolower(spec->qualifier) == 'z') {
		spec->type = FORMAT_TYPE_SIZE_T;
	} else if (spec->qualifier == 't') {
		spec->type = FORMAT_TYPE_PTRDIFF;
	} else if (spec->qualifier == 'H') {
		BUILD_BUG_ON(FORMAT_TYPE_UBYTE + SIGN != FORMAT_TYPE_BYTE);
		spec->type = FORMAT_TYPE_UBYTE + (spec->flags & SIGN);
	} else if (spec->qualifier == 'h') {
		BUILD_BUG_ON(FORMAT_TYPE_USHORT + SIGN != FORMAT_TYPE_SHORT);
		spec->type = FORMAT_TYPE_USHORT + (spec->flags & SIGN);
	} else {
		BUILD_BUG_ON(FORMAT_TYPE_UINT + SIGN != FORMAT_TYPE_INT);
		spec->type = FORMAT_TYPE_UINT + (spec->flags & SIGN);
	}

	return ++fmt - start;
}

/**
 * vsnprintf - Format a string and place it in a buffer
 * @buf: The buffer to place the result into
 * @size: The size of the buffer, including the trailing null space
 * @fmt: The format string to use
 * @args: Arguments for the format string
 *
 * This function follows C99 vsnprintf, but has some extensions:
 * %pS output the name of a text symbol with offset
 * %ps output the name of a text symbol without offset
 * %pF output the name of a function pointer with its offset
 * %pf output the name of a function pointer without its offset
 * %pB output the name of a backtrace symbol with its offset
 * %pR output the address range in a struct resource with decoded flags
 * %pr output the address range in a struct resource with raw flags
 * %pb output the bitmap with field width as the number of bits
 * %pbl output the bitmap as range list with field width as the number of bits
 * %pM output a 6-byte MAC address with colons
 * %pMR output a 6-byte MAC address with colons in reversed order
 * %pMF output a 6-byte MAC address with dashes
 * %pm output a 6-byte MAC address without colons
 * %pmR output a 6-byte MAC address without colons in reversed order
 * %pI4 print an IPv4 address without leading zeros
 * %pi4 print an IPv4 address with leading zeros
 * %pI6 print an IPv6 address with colons
 * %pi6 print an IPv6 address without colons
 * %pI6c print an IPv6 address as specified by RFC 5952
 * %pIS depending on sa_family of 'struct sockaddr *' print IPv4/IPv6 address
 * %piS depending on sa_family of 'struct sockaddr *' print IPv4/IPv6 address
 * %pU[bBlL] print a UUID/GUID in big or little endian using lower or upper
 *   case.
 * %*pE[achnops] print an escaped buffer
 * %*ph[CDN] a variable-length hex string with a separator (supports up to 64
 *           bytes of the input)
 * %pC output the name (Common Clock Framework) or address (legacy clock
 *     framework) of a clock
 * %pCn output the name (Common Clock Framework) or address (legacy clock
 *      framework) of a clock
 * %pCr output the current rate of a clock
 * %n is ignored
 *
 * The return value is the number of characters which would
 * be generated for the given input, excluding the trailing
 * '\0', as per ISO C99. If you want to have the exact
 * number of characters written into @buf as return value
 * (not including the trailing '\0'), use vscnprintf(). If the
 * return is greater than or equal to @size, the resulting
 * string is truncated.
 */
int vsnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
	unsigned long long num;
	char *str, *end;
	struct printf_spec spec = {0};

	if (WARN_ON_ONCE(size > INT_MAX))
		return 0;

	str = buf;
	end = buf + size;

	/* Make sure end is always >= buf */
	if (end < buf) {
		end = ((void *)-1);
		size = end - buf;
	}

	while (*fmt) {
		const char *old_fmt = fmt;
		int read = format_decode(fmt, &spec);

		fmt += read;

		switch (spec.type) {
		case FORMAT_TYPE_NONE: {
			int copy = read;
			if (str < end) {
				if (copy > end - str)
					copy = end - str;
				memcpy(str, old_fmt, copy);
			}
			str += read;
			break;
		}

		case FORMAT_TYPE_WIDTH:
			spec.field_width = va_arg(args, int);
			break;

		case FORMAT_TYPE_PRECISION:
			spec.precision = va_arg(args, int);
			break;

		case FORMAT_TYPE_CHAR: {
			char c;

			if (!(spec.flags & LEFT)) {
				while (--spec.field_width > 0) {
					if (str < end)
						*str = ' ';
					++str;

				}
			}
			c = (unsigned char) va_arg(args, int);
			if (str < end)
				*str = c;
			++str;
			while (--spec.field_width > 0) {
				if (str < end)
					*str = ' ';
				++str;
			}
			break;
		}

		case FORMAT_TYPE_STR:
			str = string(str, end, va_arg(args, char *), spec);
			break;

		case FORMAT_TYPE_PTR:
			str = pointer(fmt, str, end, va_arg(args, void *),
				      spec);
			while (isalnum(*fmt))
				fmt++;
			break;

		case FORMAT_TYPE_PERCENT_CHAR:
			if (str < end)
				*str = '%';
			++str;
			break;

		case FORMAT_TYPE_INVALID:
			if (str < end)
				*str = '%';
			++str;
			break;

		default:
			switch (spec.type) {
			case FORMAT_TYPE_LONG_LONG:
				num = va_arg(args, long long);
				break;
			case FORMAT_TYPE_ULONG:
				num = va_arg(args, unsigned long);
				break;
			case FORMAT_TYPE_LONG:
				num = va_arg(args, long);
				break;
			case FORMAT_TYPE_SIZE_T:
				if (spec.flags & SIGN)
					num = va_arg(args, ssize_t);
				else
					num = va_arg(args, size_t);
				break;
			case FORMAT_TYPE_PTRDIFF:
				num = va_arg(args, ptrdiff_t);
				break;
			case FORMAT_TYPE_UBYTE:
				num = (unsigned char) va_arg(args, int);
				break;
			case FORMAT_TYPE_BYTE:
				num = (signed char) va_arg(args, int);
				break;
			case FORMAT_TYPE_USHORT:
				num = (unsigned short) va_arg(args, int);
				break;
			case FORMAT_TYPE_SHORT:
				num = (short) va_arg(args, int);
				break;
			case FORMAT_TYPE_INT:
				num = (int) va_arg(args, int);
				break;
			default:
				num = va_arg(args, unsigned int);
			}

			str = number(str, end, num, spec);
		}
	}

	if (size > 0) {
		if (str < end)
			*str = '\0';
		else
			end[-1] = '\0';
	}

	/* the trailing null byte doesn't count towards the total */
	return str-buf;

}
EXPORT_SYMBOL(vsnprintf);

/**
 * vscnprintf - Format a string and place it in a buffer
 * @buf: The buffer to place the result into
 * @size: The size of the buffer, including the trailing null space
 * @fmt: The format string to use
 * @args: Arguments for the format string
 *
 * The return value is the number of characters which have been written into
 * the @buf not including the trailing '\0'. If @size is == 0 the function
 * returns 0.
 *
 * If you're not already dealing with a va_list consider using scnprintf().
 *
 * See the vsnprintf() documentation for format string extensions over C99.
 */
int vscnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
	int i;

	i = vsnprintf(buf, size, fmt, args);

	if (likely(i < size))
		return i;
	if (size != 0)
		return size - 1;
	return 0;
}
EXPORT_SYMBOL(vscnprintf);

/**
 * snprintf - Format a string and place it in a buffer
 * @buf: The buffer to place the result into
 * @size: The size of the buffer, including the trailing null space
 * @fmt: The format string to use
 * @...: Arguments for the format string
 *
 * The return value is the number of characters which would be
 * generated for the given input, excluding the trailing null,
 * as per ISO C99.  If the return is greater than or equal to
 * @size, the resulting string is truncated.
 */
int snprintf(char *buf, size_t size, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vsnprintf(buf, size, fmt, args);
	va_end(args);

	return i;
}
EXPORT_SYMBOL(snprintf);

/**
 * scnprintf - Format a string and place it in a buffer
 * @buf: The buffer to place the result into
 * @size: The size of the buffer, including the trailing null space
 * @fmt: The format string to use
 * @...: Arguments for the format string
 *
 * The return value is the number of characters written into @buf not including
 * the trailing '\0'. If @size is == 0 the function returns 0.
 */
int scnprintf(char *buf, size_t size, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vscnprintf(buf, size, fmt, args);
	va_end(args);

	return i;
}
EXPORT_SYMBOL(scnprintf);

/**
 * vsprintf - Format a string and place it in a buffer
 * @buf: The buffer to place the result into
 * @fmt: The format string to use
 * @args: Arguments for the format string
 *
 * The function returns the number of characters written
 * into @buf. Use vsnprintf() or vscnprintf() in order to avoid
 * buffer overflows.
 */
int vsprintf(char *buf, const char *fmt, va_list args)
{
	return vsnprintf(buf, INT_MAX, fmt, args);
}
EXPORT_SYMBOL(vsprintf);

/**
 * sprintf - Format a string and place it in a buffer
 * @buf: The buffer to place the result into
 * @fmt: The format string to use
 * @...: Arguments for the format string
 *
 * The function returns the number of characters written
 * into @buf. Use snprintf() or scnprintf() in order to avoid
 * buffer overflows.
 */
int sprintf(char *buf, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vsnprintf(buf, INT_MAX, fmt, args);
	va_end(args);

	return i;
}
EXPORT_SYMBOL(sprintf);
