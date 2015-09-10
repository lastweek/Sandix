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

#include <sandix/compiler.h>
#include <sandix/types.h>

/* Arch Specific String Functions */
#include <asm/string.h>	

#ifndef __HAVE_ARCH_STRCPY
char *strcpy(char *,const char *);
#endif

#ifndef __HAVE_ARCH_STRNCPY
char *strncpy(char *,const char *, size_t);
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

#ifndef __HAVE_ARCH_STRLEN
size_t strlen(const char *);
#endif

#ifndef __HAVE_ARCH_STRNLEN
size_t strnlen(const char *,size_t);
#endif

#ifndef __HAVE_ARCH_MEMSET
void *memset(void *,int,size_t);
#endif

#ifndef __HAVE_ARCH_MEMCPY
void *memcpy(void *,const void *,size_t);
#endif

#endif /* _SANDIX_STRING_H_ */
