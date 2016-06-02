/*
 *	Copyright (C) 2016 Yizhou Shan <shan13@purdue.edu>
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

#ifndef _ASM_X86_CMPXCHG_32_H_
#define _ASM_X86_CMPXCHG_32_H_

/*
 * CMPXCHG8B only writes to the target if we had the previous
 * value in registers, otherwise it acts as a read and gives us the
 * "new previous" value.  That is why there is a loop.  Preloading
 * EDX:EAX is a performance optimization: in the common case it means
 * we need only one locked operation.
 *
 * A SIMD/3DNOW!/MMX/FPU 64-bit store here would require at the very
 * least an FPU save and/or %cr0.ts manipulation.
 *
 * cmpxchg8b must be used with the lock prefix here to allow the
 * instruction to be executed atomically.  We need to have the reader
 * side to see the coherent 64bit value.
 */
static inline void set_64bit(volatile u64 *ptr, u64 value)
{
	u32 low  = value;
	u32 high = value >> 32;
	u64 prev = *ptr;

	asm volatile (
		"\n1:\n\t"
		LOCK_PREFIX "cmpxchg8b %0\n\t"
		"jnz 1b"
		: "=m" (*ptr), "+A" (prev)
		: "b" (low), "c" (high)
		: "memory"
	);
}

static inline u64 __cmpxchg64(volatile u64 *ptr, u64 old, u64 new)
{
	u64 prev;
	asm volatile (
		LOCK_PREFIX "cmpxchg8b %1"
		: "=A" (prev), "+m" (*ptr)
		: "b" ((u32)new), "c" ((u32)(new >> 32)), "0" (old)
		: "memory"
	);
	return prev;
}

static inline u64 __cmpxchg64_local(volatile u64 *ptr, u64 old, u64 new)
{
	u64 prev;
	asm volatile (
		"cmpxchg8b %1"
		: "=A" (prev), "+m" (*ptr)
		: "b" ((u32)new), "c" ((u32)(new >> 32)), "0" (old)
		: "memory"
	);
	return prev;
}

#define cmpxchg64(ptr, o, n)							\
	((__typeof__(*(ptr)))__cmpxchg64((ptr), (unsigned long long)(o),	\
					 (unsigned long long)(n)))
#define cmpxchg64_local(ptr, o, n)						\
	((__typeof__(*(ptr)))__cmpxchg64_local((ptr), (unsigned long long)(o),	\
					       (unsigned long long)(n)))
#endif /* _ASM_X86_CMPXCHG_32_H_ */
