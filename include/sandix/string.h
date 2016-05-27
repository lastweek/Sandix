/*
 *	Stupid Standard C Library...
 *
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

#ifndef _SANDIX_STRING_H_
#define _SANDIX_STRING_H_

#include <sandix/types.h>
#include <sandix/compiler.h>

/*
 * Architecture specific implementations
 */
#include <asm/string.h>	

char *skip_spaces(const char *str);
char *strim(char *s);

#ifndef __HAVE_ARCH_STRLEN
size_t strlen(const char *);
#endif

#ifndef __HAVE_ARCH_STRNLEN
size_t strnlen(const char *,size_t);
#endif

#ifndef __HAVE_ARCH_STRNCASECMP
int strncasecmp(const char *, const char *, size_t);
#endif

#ifndef __HAVE_ARCH_STRCASECMP
int strcasecmp(const char *, const char *);
#endif

#ifndef __HAVE_ARCH_STRCPY
char *strcpy(char *,const char *);
#endif

#ifndef __HAVE_ARCH_STRNCPY
char *strncpy(char *,const char *, size_t);
#endif

#ifndef __HAVE_ARCH_STRLCPY
size_t strlcpy(char *, const char *, size_t);
#endif

#ifndef __HAVE_ARCH_STRCAT
char *strcat(char *, const char *);
#endif

#ifndef __HAVE_ARCH_STRNCAT
char *strncat(char *, const char *, size_t);
#endif

#ifndef __HAVE_ARCH_STRCMP
int strcmp(const char *,const char *);
#endif

#ifndef __HAVE_ARCH_STRNCMP
int strncmp(const char *,const char *,size_t);
#endif

#ifndef __HAVE_ARCH_STRCHR
char *strchr(const char *, int);
#endif

#ifndef __HAVE_ARCH_STRRCHR
char *strrchr(const char *, int);
#endif

#ifndef __HAVE_ARCH_STRNCHR
char *strnchr(const char *, size_t, int);
#endif

#ifndef __HAVE_ARCH_MEMSET
void *memset(void *,int,size_t);
#endif

#ifndef __HAVE_ARCH_MEMCPY
void *memcpy(void *,const void *,size_t);
#endif

#ifndef __HAVE_ARCH_MEMMOVE
void *memmove(void *, const void *, size_t);
#endif

#ifndef __HAVE_ARCH_MEMCMP
int memcmp(const void *, const void *, size_t);
#endif

#ifndef __HAVE_ARCH_MEMSCAN
void *memscan(void *, int, size_t);
#endif

#ifndef __HAVE_ARCH_STRSTR
char *strstr(const char *, const char *);
#endif

#ifndef __HAVE_ARCH_STRNSTR
char *strnstr(const char *, const char *, size_t);
#endif

#ifndef __HAVE_ARCH_MEMCHR
void *memchr(const void *, int, size_t);
#endif

#endif /* _SANDIX_STRING_H_ */
