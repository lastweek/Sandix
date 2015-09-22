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

#ifndef _ASM_X86_SWAB_H_
#define _ASM_X86_SWAB_H_

#ifndef _SANDIX_SWAB_H_
# error INCLUDE ERROR: Do Not Include This File Directly.
#endif

#include <sandix/compiler.h>
#include <sandix/types.h>

INLINE __attribute_const __u32 __arch_swab32(__u32 val)
{
	asm volatile (
		"bswapl %0"
		: "=r" (val)
		: "0" (val)
	);
	return val;
}
#define __arch_swab32 __arch_swab32

/* Life is easier in x86_64, bswapq %0 */
INLINE __attribute_const __u64 __arch_swab64(__u64 val)
{
	union {
		struct {
			__u32 a;
			__u32 b;
		} s;
		__u64 u;
	} v;
	
	v.u = val;
	asm volatile (
		"bswapl %0"
		"bswapl %1"
		"xchgl %0,%1"
		: "=r" (v.s.a), "=r" (v.s.b)
		: "0" (v.s.a), "1" (v.s.b)
	);
	return v.u;
}
#define __arch_swab64 __arch_swab64

#endif /* _ASM_X86_SWAB_H_ */
