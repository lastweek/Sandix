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

#include <stdarg.h>

#include <asm/byteorder.h>

#include <sandix/io.h>
#include <sandix/bug.h>
#include <sandix/page.h>
#include <sandix/types.h>
#include <sandix/ctype.h>
#include <sandix/kernel.h>
#include <sandix/string.h>
#include <sandix/export.h>
#include <sandix/compiler.h>

/*
 * FLAGS KEEP THIS ORDER!
 */
#define SIGN		0x01		/* unsigned/signed */
#define LEFT		0x02		/* left justified */
#define PLUS		0x04		/* show plus */
#define SPACE		0x08		/* space if plus */
#define ZEROPAD		0x10		/* pad with zero, must be 16 == '0' - ' ' */
#define SMALL		0x20		/* use lowercase in hex (must be 32 == 0x20) */
#define SPECIAL		0x40		/* prefix hex with "0x", octal with "0" */

/*
 * FORMAT TYPE KEEP THIS ORDER!
 */
enum format_type {
	FORMAT_TYPE_NONE,		/* Just a string part */
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
	unsigned char	type;			/* format_type enum */
	unsigned char	flags;			/* flags to number() */
	unsigned char	base;			/* number base, 8, 10 or 16 only */
	unsigned char	modifier;		/* number modifier, one of 'hHlLtzZ' */
	signed short	field_width;		/* width of output field */
	signed short	precision;		/* # of digits/chars */
};

static __noinline_for_stack int skip_atoi(const char **s)
{
	int i = 0;

	do {
		i = i * 10 + *((*s)++) - '0';
	} while (isdigit(**s));

	return i;
}

/*
 * Black Magic Show Time.
 */

/*
 * Decimal conversion is by far the most typical. This directly impacts
 * e.g. top performance with many processes running. We optimize it for
 * speed by emitting two characters at a time, using a 200 byte lookup
 * table. This roughly halves the number of multiplications compared to
 * computing the digits one at a time. Implementation strongly inspired
 * by the previous version, which in turn used ideas described at
 * <http://www.cs.uiowa.edu/~jones/bcd/divide.html> (with permission
 * from the author, Douglas W. Jones).
 *
 * It turns out there is precisely one 26 bit fixed-point
 * approximation a of 64/100 for which x/100 == (x * (u64)a) >> 32
 * holds for all x in [0, 10^8-1], namely a = 0x28f5c29. The actual
 * range happens to be somewhat larger (x <= 1073741898), but that's
 * irrelevant for our purpose.
 *
 * For dividing a number in the range [10^4, 10^6-1] by 100, we still
 * need a 32x32->64 bit multiply, so we simply use the same constant.
 *
 * For dividing a number in the range [100, 10^4-1] by 100, there are
 * several options. The simplest is (x * 0x147b) >> 19, which is valid
 * for all x <= 43698.
 */

static const u16 decpair[100] = {
#define _(x) (u16) cpu_to_le16(((x % 10) | ((x / 10) << 8)) + 0x3030)
	_( 0), _( 1), _( 2), _( 3), _( 4), _( 5), _( 6), _( 7), _( 8), _( 9),
	_(10), _(11), _(12), _(13), _(14), _(15), _(16), _(17), _(18), _(19),
	_(20), _(21), _(22), _(23), _(24), _(25), _(26), _(27), _(28), _(29),
	_(30), _(31), _(32), _(33), _(34), _(35), _(36), _(37), _(38), _(39),
	_(40), _(41), _(42), _(43), _(44), _(45), _(46), _(47), _(48), _(49),
	_(50), _(51), _(52), _(53), _(54), _(55), _(56), _(57), _(58), _(59),
	_(60), _(61), _(62), _(63), _(64), _(65), _(66), _(67), _(68), _(69),
	_(70), _(71), _(72), _(73), _(74), _(75), _(76), _(77), _(78), _(79),
	_(80), _(81), _(82), _(83), _(84), _(85), _(86), _(87), _(88), _(89),
	_(90), _(91), _(92), _(93), _(94), _(95), _(96), _(97), _(98), _(99),
#undef _
};

/*
 * This will print a single '0' even if r == 0, since we would
 * immediately jump to out_r where two 0s would be written but only
 * one of them accounted for in buf. This is needed by ip4_string
 * below. All other callers pass a non-zero value of r.
*/
static __noinline_for_stack char *put_dec_trunc8(char *buf, unsigned r)
{
	unsigned q;

	/* 1 <= r < 10^8 */
	if (r < 100)
		goto out_r;

	/* 100 <= r < 10^8 */
	q = (r * (u64)0x28f5c29) >> 32;
	*((u16 *)buf) = decpair[r - 100*q];
	buf += 2;

	/* 1 <= q < 10^6 */
	if (q < 100)
		goto out_q;

	/*  100 <= q < 10^6 */
	r = (q * (u64)0x28f5c29) >> 32;
	*((u16 *)buf) = decpair[q - 100*r];
	buf += 2;

	/* 1 <= r < 10^4 */
	if (r < 100)
		goto out_r;

	/* 100 <= r < 10^4 */
	q = (r * 0x147b) >> 19;
	*((u16 *)buf) = decpair[r - 100*q];
	buf += 2;
out_q:
	/* 1 <= q < 100 */
	r = q;
out_r:
	/* 1 <= r < 100 */
	*((u16 *)buf) = decpair[r];
	buf += r < 10 ? 1 : 2;
	return buf;
}

static void put_dec_full4(char *buf, unsigned r)
{
	unsigned q;

	/* 0 <= r < 10^4 */
	q = (r * 0x147b) >> 19;
	*((u16 *)buf) = decpair[r - 100*q];
	buf += 2;
	/* 0 <= q < 100 */
	*((u16 *)buf) = decpair[q];
}

/*
 * Call put_dec_full4 on x % 10000, return x / 10000.
 * The approximation x/10000 == (x * 0x346DC5D7) >> 43
 * holds for all x < 1,128,869,999.  The largest value this
 * helper will ever be asked to convert is 1,125,520,955.
 * (second call in the put_dec code, assuming n is all-ones).
 */
static __noinline_for_stack unsigned put_dec_helper4(char *buf, unsigned x)
{
        uint32_t q = (x * (uint64_t)0x346DC5D7) >> 43;

        put_dec_full4(buf, x - q * 10000);
        return q;
}

/*
 * Based on code by Douglas W. Jones found at
 * <http://www.cs.uiowa.edu/~jones/bcd/decimal.html#sixtyfour>
 * (with permission from the author).
 * Performs no 64-bit division and hence should be fast on 32-bit machines.
 */
static char *put_dec(char *buf, unsigned long long n)
{
	unsigned int d3, d2, d1, q, h;

	if (n < 100*1000*1000)
		return put_dec_trunc8(buf, n);

	d1  = ((unsigned int)n >> 16);	/* implicit "& 0xffff" */
	h   = (n >> 32);
	d2  = (h      ) & 0xffff;
	d3  = (h >> 16);		/* implicit "& 0xffff" */

	/*
	 * n =               2^48 d3 +
	 *                   2^32 d2 +
	 *                   2^16 d1 +
	 *                        d0
	 *
	 * n = 281_4749_7671_0656 d3 +
	 *           42_9496_7296 d2 +
	 *                 6_5536 d1 +
	 *                        d0
	 */

	q   = 656 * d3 + 7296 * d2 + 5536 * d1 + ((unsigned int)n & 0xffff);
	q = put_dec_helper4(buf, q);

	q += 7671 * d3 + 9496 * d2 + 6 * d1;
	q = put_dec_helper4(buf+4, q);

	q += 4749 * d3 + 42 * d2;
	q = put_dec_helper4(buf+8, q);

	q += 281 * d3;
	buf += 12;
	if (q)
		buf = put_dec_trunc8(buf, q);
	else while (buf[-1] == '0')
		--buf;

	return buf;
}

static char *number(char *buf, char *end, unsigned long long num,
		    struct printf_spec spec)
{
	/* put_dec requires 2-byte alignment of the buffer. */
	char tmp[3 * sizeof(num)] __aligned(2);
	char sign, locase;
	int need_pfx, i;
	bool is_zero;

	/*
	 * locase = 0 or 0x20. Hence ORing digits or letters with 'locase'
	 * will produces same digits or (maybe lowercased) letters. Used
	 * when generating strings below.
	 */
	locase = (spec.flags & SMALL);

	if (spec.flags & LEFT)
		spec.flags &= ~ZEROPAD;
	
	sign = 0;
	if (spec.flags & SIGN) {
		if ((signed long long)num < 0) {
			sign = '-';
			num = -(signed long long)num;
			spec.field_width--;
		} else if (spec.flags & PLUS) {
			sign = '+';
			spec.field_width--;
		} else if (spec.flags & SPACE) {
			sign = ' ';
			spec.field_width--;
		}
	}

	/* leading prefix '0x', '0X'... */
	is_zero = (num == 0LL);
	need_pfx = ((spec.flags & SPECIAL) && spec.base != 10);
	if (need_pfx) {
		if (spec.base == 16)
			spec.field_width -= 2;
		else if (!is_zero)
			spec.field_width--;
	}

	/* Generating digit string reversely */
	i = 0;
	if (num < spec.base)
		tmp[i++] = hex_asc_upper[num] | locase;
	else if (spec.base == 10)
		i = put_dec(tmp, num) - tmp;
	else {
		int mask = spec.base - 1;
		int shift = 3;
		if (spec.base == 16)
			shift = 4;
		do {
			tmp[i++] = (hex_asc_upper[((unsigned char)num) & mask] | locase);
			num >>= shift;
		} while (num);
	}

	/* Printing 100 using %2d gives "100", not "00" */
	if (i > spec.precision)
		spec.precision = i;
	
	/* Leading SPACE padding */
	spec.field_width -= spec.precision;
	if (!(spec.flags & (ZEROPAD | LEFT))) {
		while (--spec.field_width >= 0) {
			if (buf < end)
				*buf = ' ';
			++buf;
		}
	}
	
	/* Sign padding */
	if (sign) {
		if (buf < end)
			*buf = sign;
		++buf;
	}

	/* "0x" / "0" PREFIX padding */
	if (need_pfx) {
		if (spec.base == 16 || !is_zero) {
			if (buf < end)
				*buf = '0';
			++buf;
		}
		if (spec.base == 16) {
			if (buf < end)
				*buf = ('X' | locase);
			++buf;
		}
	}
	
	/* Final ZERO or SPACE padding */
	if (!(spec.flags & LEFT)) {
		char c = ' ' + (spec.flags & ZEROPAD);
		while (--spec.field_width >= 0) {
			if (buf < end)
				*buf = c;
			++buf;
		}
	}
	
	/* Even more zero padding? */
	while (i <= --spec.precision) {
		if (buf < end)
			*buf = '0';
		++buf;
	}
	
	/* Actual digit string padding */
	while (--i >= 0) {
		if (buf < end)
			*buf = tmp[i];
		++buf;
	}
	
	/* Trailing space padding */
	while (--spec.field_width >= 0) {
		if (buf < end)
			*buf = ' ';
		++buf;
	}

	return buf;
}

static char *string(char *buf, char *end, const char *s,
		    struct printf_spec spec)
{
	int len, i;

	if ((unsigned long)s < PAGE_SIZE)
		s = "(null)";

	len = strnlen(s, spec.precision);

	if (!(spec.flags & LEFT)) {
		while (len < spec.field_width--) {
			if (buf < end)
				*buf = ' ';
			++buf;
		}
	}
	for (i = 0; i < len; ++i) {
		if (buf < end)
			*buf = *s;
		++buf; ++s;
	}
	while (len < spec.field_width--) {
		if (buf < end)
			*buf = ' ';
		++buf;
	}

	return buf;
}

static __noinline_for_stack
char *resource_string(char *buf, char *end, struct resource *res,
		      struct printf_spec spec, const char *fmt)
{
#ifndef IO_RSRC_PRINTK_SIZE
#define IO_RSRC_PRINTK_SIZE	6
#endif

#ifndef MEM_RSRC_PRINTK_SIZE
#define MEM_RSRC_PRINTK_SIZE	10
#endif
	static const struct printf_spec io_spec = {
		.base = 16,
		.field_width = IO_RSRC_PRINTK_SIZE,
		.precision = -1,
		.flags = SPECIAL | SMALL | ZEROPAD,
	};
	static const struct printf_spec mem_spec = {
		.base = 16,
		.field_width = MEM_RSRC_PRINTK_SIZE,
		.precision = -1,
		.flags = SPECIAL | SMALL | ZEROPAD,
	};
	static const struct printf_spec bus_spec = {
		.base = 16,
		.field_width = 2,
		.precision = -1,
		.flags = SMALL | ZEROPAD,
	};
	static const struct printf_spec dec_spec = {
		.base = 10,
		.precision = -1,
		.flags = 0,
	};
	static const struct printf_spec str_spec = {
		.field_width = -1,
		.precision = 10,
		.flags = LEFT,
	};
	static const struct printf_spec flag_spec = {
		.base = 16,
		.precision = -1,
		.flags = SPECIAL | SMALL,
	};

	/* 32-bit res (sizeof==4): 10 chars in dec, 10 in hex ("0x" + 8)
	 * 64-bit res (sizeof==8): 20 chars in dec, 18 in hex ("0x" + 16) */
#define RSRC_BUF_SIZE		((2 * sizeof(resource_size_t)) + 4)
#define FLAG_BUF_SIZE		(2 * sizeof(res->flags))
#define DECODED_BUF_SIZE	sizeof("[mem - 64bit pref window disabled]")
#define RAW_BUF_SIZE		sizeof("[mem - flags 0x]")
	char sym[max(2*RSRC_BUF_SIZE + DECODED_BUF_SIZE,
		     2*RSRC_BUF_SIZE + FLAG_BUF_SIZE + RAW_BUF_SIZE)];

	char *p = sym, *pend = sym + sizeof(sym);
	int decode = (fmt[0] == 'R') ? 1 : 0;
	const struct printf_spec *specp;

	*p++ = '[';
	if (res->flags & IORESOURCE_IO) {
		p = string(p, pend, "io  ", str_spec);
		specp = &io_spec;
	} else if (res->flags & IORESOURCE_MEM) {
		p = string(p, pend, "mem ", str_spec);
		specp = &mem_spec;
	} else if (res->flags & IORESOURCE_IRQ) {
		p = string(p, pend, "irq ", str_spec);
		specp = &dec_spec;
	} else if (res->flags & IORESOURCE_DMA) {
		p = string(p, pend, "dma ", str_spec);
		specp = &dec_spec;
	} else if (res->flags & IORESOURCE_BUS) {
		p = string(p, pend, "bus ", str_spec);
		specp = &bus_spec;
	} else {
		p = string(p, pend, "??? ", str_spec);
		specp = &mem_spec;
		decode = 0;
	}
	if (decode && res->flags & IORESOURCE_UNSET) {
		p = string(p, pend, "size ", str_spec);
		p = number(p, pend, resource_size(res), *specp);
	} else {
		p = number(p, pend, res->start, *specp);
		if (res->start != res->end) {
			*p++ = '-';
			p = number(p, pend, res->end, *specp);
		}
	}
	if (decode) {
		if (res->flags & IORESOURCE_MEM_64)
			p = string(p, pend, " 64bit", str_spec);
		if (res->flags & IORESOURCE_PREFETCH)
			p = string(p, pend, " pref", str_spec);
		if (res->flags & IORESOURCE_WINDOW)
			p = string(p, pend, " window", str_spec);
		if (res->flags & IORESOURCE_DISABLED)
			p = string(p, pend, " disabled", str_spec);
	} else {
		p = string(p, pend, " flags ", str_spec);
		p = number(p, pend, res->flags, flag_spec);
	}
	*p++ = ']';
	*p = '\0';

	return string(buf, end, sym, spec);
}

static __noinline_for_stack
char *hex_string(char *buf, char *end, u8 *addr, struct printf_spec spec, const char *fmt)
{
	int i, len = 1;		/* if we pass '%ph[CDN]', field width remains
				   negative value, fallback to the default */
	char separator;

	if (spec.field_width == 0)
		/* nothing to print */
		return buf;
	
	switch (fmt[1]) {
	case 'C':
		separator = ':';
		break;
	case 'D':
		separator = '-';
		break;
	case 'N':
		separator = 0;
		break;
	default:
		separator = ' ';
		break;
	}

	if (spec.field_width > 0)
		len = min_t(int, spec.field_width, 64);

	for (i = 0; i < len; ++i) {
		if (buf < end)
			*buf = hex_asc_hi(addr[i]);
		++buf;
		if (buf < end)
			*buf = hex_asc_lo(addr[i]);
		++buf;

		if (separator && i != len - 1) {
			if (buf < end)
				*buf = separator;
			++buf;
		}
	}

	return buf;
}

static __noinline_for_stack
char *address_val(char *buf, char *end, const void *addr,
		  struct printf_spec spec, const char *fmt)
{
	unsigned long long num;

	spec.flags |= SPECIAL | SMALL | ZEROPAD;
	spec.base = 16;

	switch (fmt[1]) {
	case 'd':
		num = *(const dma_addr_t *)addr;
		spec.field_width = sizeof(dma_addr_t) * 2 + 2;
		break;
	case 'p':
	default:
		num = *(const phys_addr_t *)addr;
		spec.field_width = sizeof(phys_addr_t) * 2 + 2;
		break;
	}

	return number(buf, end, num, spec);
}

/* TODO */
static __noinline_for_stack char *pointer(const char *fmt,
					  char *buf,
					  char *end,
					  void *ptr,
					  struct printf_spec spec)
{
	const int default_width = 2 * sizeof(void *);

	if (!ptr && *fmt != 'K') {
		/*
		 * Print (null) with the same width as a pointer so it makes
		 * tabular output look nice.
		 */
		if (spec.field_width == -1)
			spec.field_width = default_width;
		return string(buf, end, "(null)", spec);
	}

	switch (*fmt) {
	case 'R':
	case 'r':
		return resource_string(buf, end, ptr, spec, fmt);
	case 'h':
		return hex_string(buf, end, ptr, spec, fmt);
	case 'a':
		return address_val(buf, end, ptr, spec, fmt);
	}
	spec.flags |= SMALL;
	if (spec.field_width == -1) {
		spec.field_width = default_width;
		spec.flags |= ZEROPAD;
	}
	spec.base = 16;

	return number(buf, end, (unsigned long) ptr, spec);
}

/*
 * Helper function to decode printf style format.
 * Each call decode a token from the format and return the
 * number of characters read (or likely the delta where it wants
 * to go on the next call).
 */
static int format_decode(const char *fmt, struct printf_spec *spec)
{
	const char *start = fmt;

	/*
	 * A field width or precision, or both, may be indicated
	 * by an asterisk `*'. In this case, an int argument supplies
	 * the field width or precision. A negative field width is
	 * treated as a left adjustment flag followed by a positive
	 * field width;
	 */

	/*
	 * Get the FIELD WIDTH and PRECISION first
	 */

	if (spec->type == FORMAT_TYPE_WIDTH) {
		if (spec->field_width < 0) {
			spec->field_width = -spec->field_width;
			spec->flags |= LEFT;
		}
		spec->type = FORMAT_TYPE_NONE;
		goto precision;
	}

	if (spec->type == FORMAT_TYPE_PRECISION) {
		if (spec->precision < 0)
			spec->precision = 0;
		spec->type = FORMAT_TYPE_NONE;
		goto modifier;
	}

	/* Default type */
	spec->type = FORMAT_TYPE_NONE;

	for (; *fmt; ++fmt) {
		if (*fmt == '%')
			break;
	}

	/* Return the current NON-FORMAT string. */
	if (fmt != start || !*fmt)
		return fmt - start;

	/*
	 * Handle the [FLAGs]
	 */

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

	/*
	 * Handle the [FIELD WIDTH]
	 */

	spec->field_width = -1;
	if (isdigit(*fmt))
		spec->field_width = skip_atoi(&fmt);
	else if (*fmt == '*') {
		/* See comment above */
		spec->type = FORMAT_TYPE_WIDTH;
		return ++fmt - start;
	}

	/*
	 * Handle the [PRECISION]
	 */

precision:
	spec->precision = -1;
	if (*fmt == '.') {
		++fmt;
		if (isdigit(*fmt)) {
			spec->precision = skip_atoi(&fmt);
			if (spec->precision < 0)
				spec->precision = 0;
		} else if (*fmt == '*') {
			spec->type = FORMAT_TYPE_PRECISION;
			return ++fmt - start;
		}
	}

	/*
	 * Handle the [LENGTH MODIFIER]
	 */

modifier:
	spec->modifier = -1;
	if (*fmt == 'h' || _tolower(*fmt) == 'l' ||
	    _tolower(*fmt) == 'z' || *fmt == 't')
	{
		spec->modifier = *fmt++;
		if (spec->modifier == *fmt)
		{
			if (spec->modifier == 'l')
			{
				spec->modifier = 'L';
				++fmt;
			}
			else if (spec->modifier == 'h')
			{
				spec->modifier = 'H';
				++fmt;
			}
		}
	}

	/* default */
	spec->base = 10;

	/*
	 * Handle the [CONVERSION SPECIFIERS]
	 */

	switch (*fmt) {
	/*
	 * Length modifiers have no effect on following conversion specifiers:
	 */
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
	/*
	 * Lengh modifiers are applied to the following conversion specifiers:
	 */
	case 'd':
	case 'i':
		spec->flags |= SIGN;
	case 'u':
		break;
	case 'x':
		spec->flags |= SMALL;
	case 'X':
		spec->base = 16;
		break;
	case 'o':
		spec->base = 8;
		break;
	case 'n':
		/*
		 * Since %n poses a greater security risk than utility.
		 * Treat it as an invalid format specifier.
		 */
	default:
		spec->type = FORMAT_TYPE_INVALID;
		return fmt - start;
	}

	/*
	 * Apply modifers, determine the final type
	 */

	if (spec->modifier == 'L')
		spec->type = FORMAT_TYPE_LONG_LONG;
	else if (spec->modifier == 'l')
		spec->type = FORMAT_TYPE_ULONG + (spec->flags & SIGN);
	else if (_tolower(spec->modifier) == 'z')
		spec->type = FORMAT_TYPE_SIZE_T;
	else if (spec->modifier == 't')
		spec->type = FORMAT_TYPE_PTRDIFF;
	else if (spec->modifier == 'H')
		spec->type = FORMAT_TYPE_UBYTE + (spec->flags & SIGN);
	else if (spec->modifier == 'h')
		spec->type = FORMAT_TYPE_USHORT + (spec->flags & SIGN);
	else
		spec->type = FORMAT_TYPE_UINT + (spec->flags & SIGN);

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

	if (size > INT_MAX)
		return 0;

	str = buf;
	end = buf + size;

	/* Make sure size >=0 */
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
			c = (unsigned char)va_arg(args, int);
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

	/* The '\0' does not count into the total. */
	return str - buf;

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
