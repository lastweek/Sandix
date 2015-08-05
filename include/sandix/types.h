/*
 *	include/sandix/types.h - General kernel definitions
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

#ifndef _SANDIX_TYPES_H_
#define _SANDIX_TYPES_H_

#ifndef __ASSEMBLY__

#ifndef	NULL
#define	NULL	((void *)0)
#endif

#define	bool int
enum {
	false,
	true
};

typedef unsigned long long	u64;
typedef unsigned int		u32;
typedef unsigned short		u16;
typedef unsigned char		u8;
typedef unsigned long long	__u64;
typedef unsigned int		__u32;
typedef unsigned short		__u16;
typedef unsigned char		__u8;

/*
 * Some POSIX definitons???
 */
typedef unsigned int pit_t;
typedef unsigned int phys_addr_t;

/*
 * Use size_t to count number of bytes
 * Use ssize_t to count number of bytes or return negtive error value.
 */
typedef unsigned int size_t;
typedef int ssize_t;

/*
 * Use typedef struct to avoid some programming mistake. If typedef int atomic_t
 * directly, then compiler will not complain about your incorrect usage.
 */
typedef struct {
	int counter;
} atomic_t;

#endif /* __ASSEMBLY__ */

#endif /* _SANDIX_TYPES_H_ */
