/*
 *	include/asm/types.h - Arch Specific Fundamental Types
 *
 *	Integer declarations for whom use "long long" for 64-bit types.
 *	Of course some other architectures use "long" for 64-bit types. 
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

#ifndef _ASM_TYPES_H_
#define _ASM_TYPES_H_

#ifndef _SANDIX_TYPES_H_
# error INCLUDE ERROR: Do Not Include This File Directly.
#endif

#define __BITS_PER_LONG		32
#define __BITS_PER_LONG_LONG	64

typedef unsigned long long	__u64;
typedef signed long long	__s64;

typedef unsigned int		__u32;
typedef signed int		__s32;

typedef unsigned short		__u16;
typedef signed short		__s16;

typedef unsigned char		__u8;
typedef signed char		__s8;

typedef unsigned long long	u64;
typedef signed long long	s64;

typedef unsigned int		u32;
typedef signed int		s32;

typedef unsigned short		u16;
typedef signed short		s16;

typedef unsigned char		u8;
typedef signed char		s8;

#endif /* _ASM_TYPES_H_ */
