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

#ifndef _SANDIX_KERNEL_H_
#define _SANDIX_KERNEL_H_

#include <stdarg.h>
#include <sandix/types.h>
#include <sandix/export.h>
#include <sandix/printk.h>
#include <sandix/stddef.h>
#include <sandix/linkage.h>
#include <sandix/compiler.h>
#include <sandix/typecheck.h>

#define USHRT_MAX		((u16)(~0U))
#define SHRT_MAX		((s16)(USHRT_MAX>>1))
#define SHRT_MIN		((s16)(-SHRT_MAX - 1))
#define INT_MAX			((int)(~0U>>1))
#define INT_MIN			(-INT_MAX - 1)
#define UINT_MAX		(~0U)
#define LONG_MAX		((long)(~0UL>>1))
#define LONG_MIN		(-LONG_MAX - 1)
#define ULONG_MAX		(~0UL)
#define LLONG_MAX		((long long)(~0ULL>>1))
#define LLONG_MIN		(-LLONG_MAX - 1)
#define ULLONG_MAX		(~0ULL)
#define SIZE_MAX		(~(size_t)0)

#define U8_MAX			((u8)~0U)
#define S8_MAX			((s8)(U8_MAX>>1))
#define S8_MIN			((s8)(-S8_MAX - 1))
#define U16_MAX			((u16)~0U)
#define S16_MAX			((s16)(U16_MAX>>1))
#define S16_MIN			((s16)(-S16_MAX - 1))
#define U32_MAX			((u32)~0U)
#define S32_MAX			((s32)(U32_MAX>>1))
#define S32_MIN			((s32)(-S32_MAX - 1))
#define U64_MAX			((u64)~0ULL)
#define S64_MAX			((s64)(U64_MAX>>1))
#define S64_MIN			((s64)(-S64_MAX - 1))

/*
 * This looks more complex than it should be. But we need to
 * get the type for the ~ right in round_down (it needs to be
 * as wide as the result!), and we want to evaluate the macro
 * arguments just once each.
 */
#define __round_mask(x, y)	((__typeof__(x))((y)-1))
#define round_up(x, y)		((((x)-1) | __round_mask(x, y))+1)
#define round_down(x, y)	((x) & ~__round_mask(x, y))

#define STACK_MAGIC		0xdeadbeef

#define DIV_ROUND_UP(n,d)	(((n) + (d) - 1) / (d))

#define ARRAY_SIZE(x)		(sizeof(x) / sizeof((x)[0]))

#define _RET_IP_		(unsigned long)__builtin_return_address(0)
#define _THIS_IP_						\
({								\
__label__ __here;						\
		__here:						\
		(unsigned long)&&__here;			\
})

/*
 * min()/max() macros that also do
 * strict type-checking.. See the
 * "unnecessary" pointer comparison.
 */
#define min(x, y)				\
({						\
	typeof(x) _min1 = (x);			\
	typeof(y) _min2 = (y);			\
	(void) (&_min1 == &_min2);		\
	_min1 < _min2 ? _min1 : _min2;		\
})

#define max(x, y)				\
({						\
	typeof(x) _max1 = (x);			\
	typeof(y) _max2 = (y);			\
	(void) (&_max1 == &_max2);		\
	_max1 > _max2 ? _max1 : _max2;		\
})

#define min3(x, y, z) min((typeof(x))min(x, y), z)
#define max3(x, y, z) max((typeof(x))max(x, y), z)

/*
 * ..and if you can't take the strict
 * types, you can specify one yourself.
 *
 * Or not use min/max/clamp at all, of course.
 */
#define min_t(type, x, y)			\
({						\
	type __min1 = (x);			\
	type __min2 = (y);			\
	__min1 < __min2 ? __min1: __min2;	\
})

#define max_t(type, x, y)			\
({						\
	type __max1 = (x);			\
	type __max2 = (y);			\
	__max1 > __max2 ? __max1: __max2;	\
})

/**
 * container_of - find which structure this @ptr located
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 */
#define container_of(ptr, type, member)				\
({								\
	typeof( ((type *)0)->member ) *__ptr = (ptr);		\
	(type *)( (char *)__ptr - offsetof(type,member) );	\
})

/*
 * lib/hexdump.c
 */
extern int hex_to_bin(char ch);
extern int hex2bin(unsigned char *dst, const char *src, size_t count);
extern char *bin2hex(char *dst, const void *src, size_t count);

extern const char hex_asc[];
extern const char hex_asc_upper[];

#define hex_asc_lo(x)		hex_asc[((x) & 0x0f)]
#define hex_asc_hi(x)		hex_asc[((x) & 0xf0) >> 4]
#define hex_asc_upper_lo(x)	hex_asc_upper[((x) & 0x0f)]
#define hex_asc_upper_hi(x)	hex_asc_upper[((x) & 0xf0) >> 4]

static inline char *hex_byte_pack(char *buf, unsigned char byte)
{
	*buf++ = hex_asc_hi(byte);
	*buf++ = hex_asc_lo(byte);
	return buf;
}

static inline char *hex_byte_pack_upper(char *buf, unsigned char byte)
{
	*buf++ = hex_asc_upper_hi(byte);
	*buf++ = hex_asc_upper_lo(byte);
	return buf;
}

/*
 * lib/vsprintf.c
 */
__printf(2, 3)
int sprintf(char *buf, const char * fmt, ...);

__printf(2, 0)
int vsprintf(char *buf, const char *, va_list args);

__printf(3, 4)
int snprintf(char *buf, size_t size, const char *fmt, ...);

__printf(3, 0)
int vsnprintf(char *buf, size_t size, const char *fmt, va_list args);

__printf(3, 4)
int scnprintf(char *buf, size_t size, const char *fmt, ...);

__printf(3, 0)
int vscnprintf(char *buf, size_t size, const char *fmt, va_list args);

/*
 * kernel/panic.c
 */
__printf(1, 2) __cold __noreturn
void panic(const char *fmt, ...);

#endif /* _SANDIX_KERNEL_H_ */
