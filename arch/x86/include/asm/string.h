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

#ifndef _ASM_X86_STRING_H_
#define _ASM_X86_STRING_H_

#include <sandix/compiler.h>
#include <sandix/types.h>

#define __HAVE_ARCH_STRCPY
char *strcpy(char *dest, const char *src);

#define __HAVE_ARCH_STRNCPY
char *strncpy(char *dest, const char *src, size_t count);

#define __HAVE_ARCH_STRCAT
char *strcat(char *dest, const char *src);

#define __HAVE_ARCH_STRNCAT
char *strncat(char *dest, const char *src, size_t count);

#define __HAVE_ARCH_STRCMP
int strcmp(const char *cs, const char *ct);

#define __HAVE_ARCH_STRNCMP
int strncmp(const char *cs, const char *ct, size_t count);

#define __HAVE_ARCH_STRLEN
size_t strlen(const char *s);

#define __HAVE_ARCH_STRNLEN
size_t strnlen(const char *s, size_t count);

/*
 * It is not too hard to RTFC to understand code below.
 * We have made so much effort, it turns out GCC built-in wins.
 */
#define __HAVE_ARCH_MEMCPY
#define __HAVE_ARCH_MEMSET

ALWAYS_INLINE void *__memcpy(void *to, const void *from, size_t n)
{
	int d0, d1, d2;
	asm volatile (
		"rep ; movsl\n\t"
		"movl %4,%%ecx\n\t"
		"andl $3,%%ecx\n\t"
		"jz 1f\n\t"
		"rep ; movsb\n\t"
		"1:"
		: "=&c" (d0), "=&D" (d1), "=&S" (d2)
		: "0" (n / 4), "g" (n), "1" ((long)to), "2" ((long)from)
		: "memory"
	);
	return to;
}

/*
 * This looks very very very ugly, but GCC can optimize it totally,
 * since the count is constant. Now you can image how builtin works.
 */
ALWAYS_INLINE void *__constant_memcpy(void *to, const void *from, size_t n)
{
	long esi, edi;
	if (!n)
		return to;

	switch (n) {
	case 1:
		*(char *)to = *(char *)from;
		return to;
	case 2:
		*(short *)to = *(short *)from;
		return to;
	case 4:
		*(int *)to = *(int *)from;
		return to;
	case 3:
		*(short *)to = *(short *)from;
		*((char *)to + 2) = *((char *)from + 2);
		return to;
	case 5:
		*(int *)to = *(int *)from;
		*((char *)to + 4) = *((char *)from + 4);
		return to;
	case 6:
		*(int *)to = *(int *)from;
		*((short *)to + 2) = *((short *)from + 2);
		return to;
	case 8:
		*(int *)to = *(int *)from;
		*((int *)to + 1) = *((int *)from + 1);
		return to;
	}

	esi = (long)from;
	edi = (long)to;

	/* SYZ: Why choose 4 movsl as the base? */
	if (n >= 5 * 4) {
		/* Large block: use rep prefix */
		int ecx;
		asm volatile (
			"rep ; movsl"
			: "=&c" (ecx), "=&D" (edi), "=&S" (esi)
			: "0" (n / 4), "1" (edi), "2" (esi)
			: "memory"
		);
	} else {
		/* Small block: do not clobber ecx + smaller code */
		if (n >= 4 * 4)
			asm volatile("movsl"
				     : "=&D"(edi), "=&S"(esi)
				     : "0"(edi), "1"(esi)
				     : "memory");
		if (n >= 3 * 4)
			asm volatile("movsl"
				     : "=&D"(edi), "=&S"(esi)
				     : "0"(edi), "1"(esi)
				     : "memory");
		if (n >= 2 * 4)
			asm volatile("movsl"
				     : "=&D"(edi), "=&S"(esi)
				     : "0"(edi), "1"(esi)
				     : "memory");
		if (n >= 1 * 4)
			asm volatile("movsl"
				     : "=&D"(edi), "=&S"(esi)
				     : "0"(edi), "1"(esi)
				     : "memory");
	}

	/* The remainder */
	switch (n % 4) {
	case 0:
		return to;
	case 1:
		asm volatile("movsb"
			     : "=&D"(edi), "=&S"(esi)
			     : "0"(edi), "1"(esi)
			     : "memory");
		return to;
	case 2:
		asm volatile("movsw"
			     : "=&D"(edi), "=&S"(esi)
			     : "0"(edi), "1"(esi)
			     : "memory");
		return to;
	default:
		asm volatile("movsw\n\tmovsb"
			     : "=&D"(edi), "=&S"(esi)
			     : "0"(edi), "1"(esi)
			     : "memory");
		return to;
	}
}

#if (__GNUC__ >= 4)
# define memcpy(t, f, n)			\
	__builtin_memcpy(t, f, n)
#else
# define memcpy(t, f, n)			\
	(__builtin_constant_p((n))		\
	? __constant_memcpy((t), (f), (n))	\
	: __memcpy((t), (f), (n)))		
#endif

ALWAYS_INLINE void *__memset(void *s, char c, size_t count)
{
	int d0, d1;
	asm volatile (
		"rep ; stosb"
		: "=&c" (d0), "=&D" (d1)
		: "a" (c), "1" (s), "0" (count)
		: "memory"
	);
	return s;
}

/*
 * memset(x, 0, y) is a reasonably common thing to do, so we want to fill
 * things 32 bits at a time even when we don't know the size of the
 * area at compile-time..
 */
ALWAYS_INLINE void *__constant_c_memset(void *s, unsigned long c, size_t count)
{
	int d0, d1;
	asm volatile (
		"rep ; stosl\n\t"
		"testb $2,%b3\n\t"
		"je 1f\n\t"
		"stosw\n"
		"1:\ttestb $1,%b3\n\t"
		"je 2f\n\t"
		"stosb\n"
		"2:"
		: "=&c" (d0), "=&D" (d1)
		: "a" (c), "q" (count), "0" (count/4), "1" ((long)s)
		: "memory"
	);
	return s;
}

/*
 * This looks horribly ugly, but the compiler can optimize it totally,
 * as we by now know that both pattern and count is constant..
 */
ALWAYS_INLINE void *__constant_c_and_count_memset(void *s,
						  unsigned long pattern,
						  size_t count)
{
	switch (count) {
	case 0:
		return s;
	case 1:
		*(unsigned char *)s = pattern & 0xff;
		return s;
	case 2:
		*(unsigned short *)s = pattern & 0xffff;
		return s;
	case 3:
		*(unsigned short *)s = pattern & 0xffff;
		*((unsigned char *)s + 2) = pattern & 0xff;
		return s;
	case 4:
		*(unsigned long *)s = pattern;
		return s;
	}

#define COMMON(x)							\
	asm volatile("rep ; stosl"					\
		     x							\
		     : "=&c" (d0), "=&D" (d1)				\
		     : "a" (eax), "0" (count/4), "1" ((long)s)	\
		     : "memory")

	{
		int d0, d1;
#if __GNUC__ == 4 && __GNUC_MINOR__ == 0
		/* Workaround for broken gcc 4.0 */
		register unsigned long eax asm("%eax") = pattern;
#else
		unsigned long eax = pattern;
#endif

		switch (count % 4) {
		case 0:
			COMMON("");
			return s;
		case 1:
			COMMON("\n\tstosb");
			return s;
		case 2:
			COMMON("\n\tstosw");
			return s;
		default:
			COMMON("\n\tstosw\n\tstosb");
			return s;
		}
	}

#undef COMMON
}

#define __constant_c_x_memset(s, c, count)			\
	(__builtin_constant_p(count)				\
	? __constant_c_and_count_memset((s), (c), (count))	\
	: __constant_c_memset((s), (c), (count)))


#if (__GNUC__ >= 4)
# define memset(s, c, count)						\
	__builtin_memset(s, c, count)
#else
# define memset(s, c, count)						\
	(__builtin_constant_p((c))					\
	? __constant_c_x_memset((s), (0x01010101UL*(u8)(c)), (count))	\
	: __memset((s), (c), (count)))
#endif

#endif /* _ASM_X86_STRING_H_ */
