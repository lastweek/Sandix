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

#ifndef _SANDIX_TYPES_H_
#define _SANDIX_TYPES_H_

#ifndef __ASSEMBLY__

#include <sandix/posix_types.h>
#include <sandix/stddef.h>
#include <asm/types.h>

#define BITS_PER_LONG		__BITS_PER_LONG
#define BITS_PER_LONG_LONG	__BITS_PER_LONG_LONG

typedef _Bool			bool;

typedef unsigned short		umode_t;
typedef __u32			nlink_t;
typedef __kernel_dev_t		dev_t;
typedef __kernel_ino_t		ino_t;
typedef __kernel_mode_t		mode_t;
typedef __kernel_off_t		off_t;
typedef __kernel_loff_t		loff_t;
typedef __kernel_pid_t		pid_t;
typedef __kernel_time_t		time_t;
typedef __kernel_clock_t	clock_t;
typedef __kernel_timer_t	timer_t;
typedef __kernel_clockid_t	clockid_t;
typedef __kernel_caddr_t	cadder_t;
typedef __kernel_daddr_t	daddr_t;
typedef __kernel_suseconds_t	suseconds_t;
typedef __kernel_uid32_t	uid_t;
typedef __kernel_gid32_t	gid_t;
typedef __kernel_uid16_t        uid16_t;
typedef __kernel_gid16_t        gid16_t;
typedef __kernel_size_t		size_t;
typedef __kernel_ssize_t	ssize_t;
typedef __kernel_ptrdiff_t	ptrdiff_t;

/* BSD */
typedef unsigned char		u_char;
typedef unsigned short		u_short;
typedef unsigned int		u_int;
typedef unsigned long		u_long;

/* SYS V */
typedef unsigned char		unchar;
typedef unsigned short		ushort;
typedef unsigned int		uint;
typedef unsigned long		ulong;

/* Byte Order */
typedef		__u16		__be16;
typedef		__u16		__le16;
typedef		__u32		__be32;
typedef		__u32		__le32;
typedef		__u64		__be64;
typedef		__u64		__le64;

#ifndef __BIT_TYPES_DEFINED__
#define __BIT_TYPES_DEFINED__
typedef		__u8		u_int8_t;
typedef		__s8		int8_t;
typedef		__u16		u_int16_t;
typedef		__s16		int16_t;
typedef		__u32		u_int32_t;
typedef		__s32		int32_t;
#endif

typedef		__u8		uint8_t;
typedef		__u16		uint16_t;
typedef		__u32		uint32_t;

#if defined(__GNUC__)
typedef		__u64		uint64_t;
typedef		__u64		u_int64_t;
typedef		__s64		int64_t;
#endif

/* Special 64bit data type that is 8-byte aligned */
#define aligned_u64	__u64	__attribute__((aligned(8)))
#define aligned_be64	__be64	__attribute__((aligned(8)))
#define aligned_le64	__le64	__attribute__((aligned(8)))

/*
 * The type used for indexing onto a disc or disc partition.
 *
 * Sandix always considers sectors to be 512 bytes long independently
 * of the devices real block size.
 *
 * blkcnt_t is the type of the inode's block count.
 */
#ifdef CONFIG_LBDAF
typedef u64 sector_t;
typedef u64 blkcnt_t;
#else
typedef unsigned long sector_t;
typedef unsigned long blkcnt_t;
#endif

typedef u32 phys_addr_t;
typedef phys_addr_t resource_size_t;

typedef struct {
	int counter;
} atomic_t;

struct list_head {
	struct list_head *next, *prev;
};

struct hlist_node {
	struct hlist_node *next, **pprev;
};

struct hlist_head {
	struct hlist_node *first;
};

#endif /* __ASSEMBLY__ */
#endif /* _SANDIX_TYPES_H_ */
