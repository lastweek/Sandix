/*
 *	include/sandix/compiler.h - I mean, GCC...
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

#ifndef _SANDIX_COMPILER_H_
#define _SANDIX_COMPILER_H_

#define GCC_VERSION (__GNUC__ * 10000 \
		   + __GNUC_MINOR__ * 100 \
		   + __GNUC_PATCHLEVEL__)

/*
 * Many architecture use 64 bytes cache line
 * Use this size to align some hot data.
 */
#define L1_CACHE_LINE_SIZE	64

/*
 * Normally, if one byte in a cache line is diry, then the entire
 * cache line will need to be flushed back. Put the read most, which also,
 * write rarely data together, then a lot of useless cache line write
 * events can be avoided.
 */
#define __section(S)		__attribute__((__section__(#S)))
#define __read_mostly		__section(.data..read_mostly)
#define __init			__section(.init.text)
#define __initdata		__section(.init.data)

#define __aligned(x)            __attribute__((aligned(x)))
#define __printf(a, b)		__attribute__((format(printf, a, b)))
#define __scanf(a, b)		__attribute__((format(scanf, a, b)))
#define __cold			__attribute__((cold))

#define __user
#define __iomem

#define likely(x)		__builtin_expect(!!(x), 1)
#define unlikely(x)		__builtin_expect(!!(x), 0)

#define __same_type(a, b)	__builtin_types_compatible_p(typeof(a), typeof(b))
#define __unreachable()		__builtin_unreachable()
#define __constant(exp)		__builtin_constant_(exp)

#define __always_inline		inline __attribute__((always_inline))
#define INLINE			static inline
#define ALWAYS_INLINE		static __always_inline

/*
 * It is a compiler barrier.
 * It is _not_ a cpu barrier.
 */
#define barrier()		asm volatile("": : :"memory")

#endif /* _SANDIX_COMPILER_H_ */
