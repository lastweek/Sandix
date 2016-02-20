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

#ifndef _SANDIX_POSIX_TYPES_H_
#define _SANDIX_POSIX_TYPES_H_

#ifndef _SANDIX_TYPES_H_
# error "Please do not include this file directly"
#endif

#include <asm/types.h>		/* For __BITS_PER_LONG */
#include <asm/posix_types.h>	/* Architecture-Dependent Posix Types */

/*
 * Types maybe architecture-dependent.
 */

#ifndef __kernel_long_t
typedef long			__kernel_long_t;
typedef unsigned long		__kernel_ulong_t;
#endif

#ifndef __kernel_ino_t
typedef __kernel_ulong_t	__kernel_ino_t;
#endif

#ifndef __kernel_mode_t
typedef unsigned int		__kernel_mode_t;
#endif

#ifndef __kernel_pid_t
typedef int			__kernel_pid_t;
#endif

#ifndef __kernel_ipc_pid_t
typedef int			__kernel_ipc_pid_t;
#endif

#ifndef __kernel_uid_t
typedef unsigned int		__kernel_uid_t;
typedef unsigned int		__kernel_gid_t;
#endif

#ifndef __kernel_suseconds_t
typedef __kernel_long_t		__kernel_suseconds_t;
#endif

#ifndef __kernel_daddr_t
typedef int			__kernel_daddr_t;
#endif

#ifndef __kernel_uid32_t
typedef unsigned int		__kernel_uid32_t;
typedef unsigned int		__kernel_gid32_t;
#endif

#ifndef __kernel_old_uid_t
typedef __kernel_uid_t		__kernel_old_uid_t;
typedef __kernel_gid_t		__kernel_old_gid_t;
#endif

#ifndef __kernel_dev_t
typedef unsigned int		__kernel_dev_t;
#endif

/*
 * MOST 32 bit architectures use "unsigned int" size_t,
 * ALL  64 bit architectures use "unsigned long" size_t.
 */
#ifndef __kernel_size_t
#if __BITS_PER_LONG == 32
typedef unsigned int		__kernel_size_t;
typedef int			__kernel_ssize_t;
typedef int			__kernel_ptrdiff_t;
#else
typedef __kernel_ulong_t	__kernel_size_t;
typedef __kernel_long_t		__kernel_ssize_t;
typedef __kernel_long_t		__kernel_ptrdiff_t;
#endif
#endif

#ifndef __kernel_fsid_t
typedef struct {
	int	val[2];
} __kernel_fsid_t;
#endif

/*
 * Anything below here should be completely generic
 */
typedef __kernel_long_t		__kernel_off_t;
typedef long long		__kernel_loff_t;
typedef __kernel_long_t		__kernel_time_t;
typedef __kernel_long_t		__kernel_clock_t;
typedef int			__kernel_timer_t;
typedef int			__kernel_clockid_t;
typedef char *			__kernel_caddr_t;
typedef unsigned short		__kernel_uid16_t;
typedef unsigned short		__kernel_gid16_t;

#endif /* _SANDIX_POSIX_TYPES_H_ */
