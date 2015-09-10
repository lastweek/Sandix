/*
 *	include/sandix/string.h - API for String Manipulation
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

#ifndef _SANDIX_STRING_H_
#define _SANDIX_STRING_H_

#include <sandix/types.h>

char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t count);
int strcmp(const char *s0, const char *s1);
int strncmp(const char *cs, const char *ct, size_t count);
size_t strlen(const char *s);
size_t strnlen(const char *s, size_t count);

void *memset(void *s, char c, size_t n);
void *memcpy(void *to, const void *from, size_t n);

#endif /* _SANDIX_STRING_H_ */
