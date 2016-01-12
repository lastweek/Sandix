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

#include <sandix/bug.h>
#include <sandix/compiler.h>
#include <sandix/export.h>
#include <sandix/linkage.h>
#include <sandix/typecheck.h>
#include <sandix/types.h>
#include <sandix/printk.h>

#include <stdarg.h>

#define USHRT_MAX	((u16)(~0U))
#define SHRT_MAX	((s16)(USHRT_MAX>>1))
#define SHRT_MIN	((s16)(-SHRT_MAX - 1))
#define INT_MAX		((int)(~0U>>1))
#define INT_MIN		(-INT_MAX - 1)
#define UINT_MAX	(~0U)
#define LONG_MAX	((long)(~0UL>>1))
#define LONG_MIN	(-LONG_MAX - 1)
#define ULONG_MAX	(~0UL)
#define LLONG_MAX	((long long)(~0ULL>>1))
#define LLONG_MIN	(-LLONG_MAX - 1)
#define ULLONG_MAX	(~0ULL)
#define SIZE_MAX	(~(size_t)0)

/**
 * offsetof - find MEMBER offset within strcut TYPE
 * @TYPE:	the type of the container struct this is embedded in
 * @MEMBER:	the name of the member within the struct
 */
#ifdef __compiler_offsetof
# define offsetof(TYPE, MEMBER)	__compiler_offsetof(TYPE, MEMBER)
#else
# define offsetof(TYPE, MEMBER)	((unsigned int)&((TYPE *)0)->MEMBER)
#endif

/**
 * container_of - find which structure this @ptr located
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 */
#define container_of(ptr, type, member) ({			\
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
