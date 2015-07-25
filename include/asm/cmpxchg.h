#ifndef _ASM_CMPXCHG_H_
#define _ASM_CMPXCHG_H_

#include <sandix/types.h>
#include <asm/asm.h>

/*
 * Non-existant functions to indicate usage errors at link time
 */
extern void __xchg_wrong_size(void)
extern void __cmpxchg_wrong_size(void)

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
		__cmpxchg_wrong_size();					\
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


#define cmpxchg(ptr, old, new)						\
	__raw_cmpxchg((ptr), (old), (new), sizeof(*(ptr)), LOCK_PREFIX)

#define cmpxchg_local(ptr, old, new)					\
	__raw_cmpxchg((ptr), (old), (new), (size), "")


#endif /* _ASM_CMPXCHG_H_ */
