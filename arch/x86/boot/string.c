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
 * Simple string library
 */

typedef unsigned int size_t;

void *memset(void *dst, int c, size_t len)
{
	char *d = (char *)dst;
	
	while (len--)
		*d++ = (char)c;
	return dst;
}

void *memcpy(void *dst, const void *src, size_t count)
{
	char *tmp = dst;
	const char *s = src;

	while (count--)
		*tmp++ = *s++;
	return dst;
}

int memcmp(const void *s1, const void *s2, size_t len)
{
	unsigned char diff;
	asm("repe; cmpsb; setnz %0"
	    : "=qm" (diff), "+D" (s1), "+S" (s2), "+c" (len));
	return diff;
}

int strcmp(const char *str1, const char *str2)
{
	const unsigned char *s1 = (const unsigned char *)str1;
	const unsigned char *s2 = (const unsigned char *)str2;
	int delta = 0;

	while (*s1 || *s2) {
		delta = *s2 - *s1;
		if (delta)
			return delta;
		s1++;
		s2++;
	}
	return 0;
}

int strncmp(const char *str1, const char *str2, size_t len)
{
	const unsigned char *s1 = (const unsigned char *)str1;
	const unsigned char *s2 = (const unsigned char *)str2;
	int delta = 0;

	while ((*s1 || *s2) && len) {
		delta = *s2- *s1;
		if (delta)
			return delta;
		s1++;
		s2++;
		len--;
	}
	return 0;
}

size_t strlen(const char *s)
{
	const char *es = s;
	
	for (es = s; *es != '\0'; es++)
		/* nothing */;
	return (es - s);
}

size_t strnlen(const char *s, size_t maxlen)
{
	const char *es = s;
	
	while (*es && maxlen) {
		es++;
		maxlen--;
	}
	return (es - s);
}
