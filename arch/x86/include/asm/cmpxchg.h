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

#ifndef _ASM_X86_CMPXCHG_H_
#define _ASM_X86_CMPXCHG_H_

#include <sandix/compiler.h>
#include <sandix/types.h>
#include <asm/asm.h>

/*
 * Non-existant functions to indicate usage errors at link time,
 * or compile time if compiler has __compiletime_error.
 */
extern void __xchg_wrong_size(void)
	__compiletime_error("Bad argument size for xchg");
extern void __cmpxchg_wrong_size(void)
	__compiletime_error("Bad argument size for cmpxchg");

#define __X86_CASE_B	1
#define __X86_CASE_W	2
#define __X86_CASE_L	4
#define __X86_CASE_Q	8

/* 
 * An exchange-type operation, which takes a value
 * and a pointer, and returns the old value.
 */
#define __xchg_op(ptr, arg, op, lock)				\
({								\
        typeof (*(ptr)) __ret = (arg);				\
	switch (sizeof(*(ptr))) {				\
	case __X86_CASE_B:					\
	{							\
		asm volatile (					\
			lock #op "b %b0, %1\n"			\
			: "+q" (__ret), "+m" (*(ptr))		\
			: : "memory", "cc"			\
		);						\
		break;						\
	}							\
	case __X86_CASE_W:					\
	{							\
		asm volatile (					\
			lock #op "w %w0, %1\n"			\
			: "+r" (__ret), "+m" (*(ptr))		\
			: : "memory", "cc"			\
		);						\
		break;						\
	}							\
	case __X86_CASE_L:					\
	{							\
		asm volatile (					\
			lock #op "l %0, %1\n"			\
			: "+r" (__ret), "+m" (*(ptr))		\
			: : "memory", "cc"			\
		);						\
		break;						\
	}							\
	case __X86_CASE_Q:					\
	{							\
		asm volatile (					\
			lock #op "q %q0, %1\n"			\
			: "+r" (__ret), "+m" (*(ptr))		\
			: : "memory", "cc"			\
		);						\
		break;						\
	}							\
	default:						\
		__ ## op ## _wrong_size();			\
	}							\
	__ret;							\
})

/*
 * Note: no "lock" prefix even on SMP: xchg always implies lock anyway.
 * Since this is generally used to protect other memory information, we
 * use "asm volatile" and "memory" clobbers to prevent gcc from moving
 * information around.
 */
#define xchg(ptr, v)	__xchg_op((ptr), (v), xchg, "")

/*
 * Atomic compare and exchange.  Compare OLD with MEM, if identical,
 * store NEW in MEM. Return the initial value in MEM. Success is
 * indicated by comparing RETURN with OLD.
 */
#define __raw_cmpxchg(ptr, old, new, size, lock)			\
({									\
	typeof(*(ptr)) __ret;						\
	typeof(*(ptr)) __old = (old);					\
	typeof(*(ptr)) __new = (new);					\
	switch (size) {							\
	case __X86_CASE_B:						\
	{								\
		volatile u8 *__ptr = (volatile u8 *)(ptr);		\
		asm volatile (						\
			lock "cmpxchgb %2,%1"				\
			: "=a" (__ret), "+m" (*__ptr)			\
			: "q" (__new), "0" (__old)			\
			: "memory"					\
		);							\
		break;							\
	}								\
	case __X86_CASE_W:						\
	{								\
		volatile u16 *__ptr = (volatile u16 *)(ptr);		\
		asm volatile (						\
			lock "cmpxchgw %2,%1"				\
			: "=a" (__ret), "+m" (*__ptr)			\
			: "q" (__new), "0" (__old)			\
			: "memory"					\
		);							\
		break;							\
	}								\
	case __X86_CASE_L:						\
	{								\
		volatile u32 *__ptr = (volatile u32 *)(ptr);		\
		asm volatile (						\
			lock "cmpxchgl %2,%1"				\
			: "=a" (__ret), "+m" (*__ptr)			\
			: "q" (__new), "0" (__old)			\
			: "memory"					\
		);							\
		break;							\
	}								\
	case __X86_CASE_Q:						\
	{								\
		volatile u64 *__ptr = (volatile u64 *)(ptr);		\
		asm volatile (						\
			lock "cmpxchgq %2,%1"				\
			: "=a" (__ret), "+m" (*__ptr)			\
			: "q" (__new), "0" (__old)			\
			: "memory"					\
		);							\
		break;							\
	}								\
	default:							\
		__cmpxchg_wrong_size();					\
	}								\
	__ret;								\
})

#define __cmpxchg(ptr, old, new, size)					\
	__raw_cmpxchg((ptr), (old), (new), (size), LOCK_PREFIX)

#define __cmpxchg_local(ptr, old, new, size)				\
	__raw_cmpxchg((ptr), (old), (new), (size), "")

#define cmpxchg(ptr, old, new)						\
	__cmpxchg(ptr, old, new, sizeof(*(ptr)))

#define cmpxchg_local(ptr, old, new)					\
	__cmpxchg_local(ptr, old, new, sizeof(*(ptr)))


#endif /* _ASM_X86_CMPXCHG_H_ */
