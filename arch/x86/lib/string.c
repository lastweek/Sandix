/*
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

/*
 * This file describes x86-platform string functions.
 */

#include <sandix/export.h>
#include <sandix/types.h>

/*
 * I have read the LKML about why using "memory"
 * clobber in all string functions including strlen().
 * As Linus said, even if you do NOT modify memory
 * in inline assembly, you should make sure GCC
 * had already flush the "dirty" data back to memory.
 * For example, when you are using strlen(), if GCC
 * has put some modified data in registers without
 * flushing back to memory before you call strlen(),
 * then a mistake happens. So it is NECESSARY to put
 * "memory" in the clobber list.
 *
 * And why all of them using local variables like d0,
 * d1 still confusing me. Maybe linux guys just dont
 * wanna writing extra registers in the clobber list
 * in order to make it clean?
 */

char *strcpy(char *dest, const char *src)
{
	int d0, d1, d2;
	asm volatile (
		"1:\n\t"
		"lodsb\n\t"
		"stosb\n\t"
		"testb %%al, %%al\n\t"
		"jne 1b"
		: "=&S"(d0), "=&D"(d1), "=a"(d2)
		: "0"(src), "1"(dest)
		: "memory"
	);
	return dest;
}
EXPORT_SYMBOL(strcpy);

char *strncpy(char *dest, const char *src, size_t count)
{
	int d0, d1, d2, d3;
	asm volatile (
		"1:\n\t"
		"decl %2\n\t"
		"js 2f\n\t"
		"lodsb\n\t"
		"stosb\n\t"
		"testb %%al,%%al\n\t"
		"jne 1b\n\t"
		"rep\n\t"
		"stosb\n\t"
		"2:"
		: "=&S"(d0), "=&D"(d1), "=&c"(d2), "=&a"(d3)
		: "0"(src), "1"(dest), "2"(count)
		: "memory"
	);
	return dest;
}
EXPORT_SYMBOL(strncpy);

int strcmp(const char *cs, const char *ct)
{
	int ret, d0, d1;
	asm volatile (
		"1:\n\t"
		"lodsb\n\t"
		"scasb\n\t"
		"jne 2f\n\t"
		"testb %%al, %%al\n\t"
		"jne 1b\n\t"
		"xorl %%eax, %%eax\n\t"
		"jmp 3f\n\t"
		"2:\n\t"
		"sbbl %%eax, %%eax\n\t"
		"orb $1, %%al\n\t"
		"3:"
		: "=&S"(d0), "=&D"(d1), "=a"(ret)
		: "0"(cs), "1"(ct)
		: "memory"
	);
	return ret;
}
EXPORT_SYMBOL(strcmp);

int strncmp(const char *cs, const char *ct, size_t count)
{
	int res;
	int d0, d1, d2;
	asm volatile (
		"1:\n\t"
		"decl %3\n\t"
		"js 2f\n\t"
		"lodsb\n\t"
		"scasb\n\t"
		"jne 3f\n\t"
		"testb %%al,%%al\n\t"
		"jne 1b\n\t"
		"2:\n\t"
		"xorl %%eax,%%eax\n\t"
		"jmp 4f\n\t"
		"3:\n\t"
		"sbbl %%eax,%%eax\n\t"
		"orb $1,%%al\n\t"
		"4:"
		: "=a"(res), "=&S"(d0), "=&D"(d1), "=&c"(d2)
		: "1"(cs), "2"(ct), "3"(count)
		: "memory"
	);
	return res;
}
EXPORT_SYMBOL(strncmp);

size_t strlen(const char *s)
{
	int d0;
	size_t len;
	asm volatile (
		"repne ; scasb"
		: "=&D"(d0), "=c"(len)
		: "0"(s), "1"(0xffffffffu), "a"(0)
		: "memory"
	);
	return ~len - 1;
}
EXPORT_SYMBOL(strlen);

size_t strnlen(const char *s, size_t count)
{
	int d0;
	int res;
	asm volatile (
		"movl %2,%0\n\t"
		"jmp 2f\n\t"
		"1:\n\t"
		"cmpb $0,(%0)\n\t"
		"je 3f\n\t"
		"incl %0\n\t"
		"2:\n\t"
		"decl %1\n\t"
		"cmpl $-1,%1\n\t"
		"jne 1b\n\t"
		"3:\n\t"
		"subl %2,%0"
		: "=a"(res), "=&d"(d0)
		: "c"(s), "1"(count)
		: "memory"
	);
	return res;
}
EXPORT_SYMBOL(strnlen);
