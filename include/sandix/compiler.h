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

/*
 * For VIM hackers, i strongly recommend you add more reserved words to
 * your syntax file. Please consult scripts/vim-syntax
 */

#ifndef _SANDIX_COMPILER_H_
#define _SANDIX_COMPILER_H_

#if defined (__INTEL_COMPILER) || defined (__clang__)
# error "Sorry, please use GCC"
#endif

#include <sandix/types.h>

#define GCC_VERSION				\
(						\
	__GNUC__		* 10000	+	\
	__GNUC_MINOR__		* 100	+	\
	__GNUC_PATCHLEVEL__			\
)

#if GCC_VERSION < 30200
# error Sorry, please upgrade GCC
#endif

/*
 * Barrier for Compiler.
 * Prevent GCC from reordering memory accesses.
 */
#define barrier()		asm volatile("": : :"memory")

/*
 * TODO Sparse
 */
#ifdef __CHECKER__
# define __user			__attribute__((noderef, address_space(1)))
# define __kernel		__attribute__((address_space(0)))
# define __safe			__attribute__((safe))
# define __force		__attribute__((force))
# define __nocast		__attribute__((nocast))
# define __iomem		__attribute__((noderef, address_space(2)))
# define __must_hold(x)		__attribute__((context(x,1,1)))
# define __acquires(x)		__attribute__((context(x,0,1)))
# define __releases(x)		__attribute__((context(x,1,0)))
# define __acquire(x)		__context__(x,1)
# define __release(x)		__context__(x,-1)
# define __cond_lock(x,c)	((c) ? ({ __acquire(x); 1; }) : 0)
# define __percpu		__attribute__((noderef, address_space(3)))
# define __pmem			__attribute__((noderef, address_space(5)))
#ifdef CONFIG_SPARSE_RCU_POINTER
# define __rcu		__attribute__((noderef, address_space(4)))
#else
# define __rcu
#endif
extern void __chk_user_ptr(const volatile void __user *);
extern void __chk_io_ptr(const volatile void __iomem *);
#else
# define __user
# define __kernel
# define __safe
# define __force
# define __nocast
# define __iomem
# define __chk_user_ptr(x) (void)0
# define __chk_io_ptr(x) (void)0
# define __builtin_warning(x, y...) (1)
# define __must_hold(x)
# define __acquires(x)
# define __releases(x)
# define __acquire(x) (void)0
# define __release(x) (void)0
# define __cond_lock(x,c) (c)
# define __percpu
# define __rcu
# define __pmem
#endif

/*
 * Is this type a native word size -- useful for atomic operations.
 */
#define __native_word(t)			\
	(sizeof(t) == sizeof(char)	||	\
	 sizeof(t) == sizeof(short)	||	\
	 sizeof(t) == sizeof(int)	||	\
	 sizeof(t) == sizeof(long))

/*
 * Many architecture use 64 bytes cache line.
 * Keep data cache line size align will improve program performance.
 * Also, this size is used to align __read_mostly section in linker.
 * See kernel/vmSandix.ld.S for more details.
 */
#define L1_CACHE_LINE_SIZE	64

/*
 * Sections - Collaborate with Linker Script
 *
 * When changing/adding section names, please, remember modify the
 * linker script to make everything right! Current design is not
 * very nice, but nobody would like to change section names often. ;)
 *
 * Why __read_mostly?
 * Normally, if one byte in a cache line is diry, then the entire
 * cache line will need to be flushed back. Put the read most, which also,
 * written rarely data together. Then a lot of meaningless cache line flush
 * events can be avoided.
 *
 * Why __init __initdata?
 * Normally, functions and variables marked as __init* are gathered into
 * the .init.* sections. Those marked ones are only used when system
 * initialize itself, and all of them is thought to be useless after init.
 * Hence, the .init.text and .init.data can be freed later to make room.
 */
#define __section(S)		__attribute__((__section__(#S)))
#define __init			__section(.init.text)
#define __initdata		__section(.init.data)
#define __read_mostly		__section(.data..read_mostly)
#define __lockfunc		__section(.spinlock.text)

/*
 * Generic GCC Function and Variable attribute.
 * Please consult GCC manual for more details.
 * Please consult <linux/compiler.h> for more details.
 */
#define __weak			__attribute__((__weak__))
#define __pure			__attribute__((__pure__))
#define __packed		__attribute__((__packed__))
#define __noreturn		__attribute__((__noreturn__))
#define __unused		__attribute__((__unused__))
#define __maybe_unused		__attribute__((__unused__))
#define __always_unused		__attribute__((__unused__))
#define __attribute_const	__attribute__((__const__))
#define __aligned(x)            __attribute__((aligned(x)))
#define __scanf(a, b)		__attribute__((format(scanf, a, b)))
#define __printf(a, b)		__attribute__((format(printf, a, b)))
#define __alias(symbol)		__attribute__((alias(#symbol)))

#define __same_type(a, b)	__builtin_types_compatible_p(typeof(a), typeof(b))
#define __constant(exp)		__builtin_constant_p(exp)

#define likely(x)		__builtin_expect(!!(x), 1)
#define unlikely(x)		__builtin_expect(!!(x), 0)

/*
 * Rather then using noinline to prevent stack consumption, use
 * noinline_for_stack instead.  For documentation reasons.
 */
#define __noinline		__attribute__((__noinline__))
#define __noinline_for_stack	__noinline

#define __always_inline		inline __attribute__((always_inline))
#define INLINE			static inline
#define ALWAYS_INLINE		static __always_inline

/*
 * GCC_VERSION specific attribute.
 * Please consult <linux/compiler-gcc.h> for more details.
 */
#if GCC_VERSION < 30300
# define __used			__attribute__((__unused__))
#else
# define __used			__attribute__((__used__))
#endif

#if GCC_VERSION >= 30400
# define __must_check		__attribute__((warn_unused_result))
#else
# define __must_check
#endif

#if GCC_VERSION >= 40000

#if GCC_VERSION >= 40100 && GCC_VERSION < 40600
# define __compiletime_object_size(obj) __builtin_object_size(obj, 0)
#else
# define __compiletime_object_size(obj) -1
#endif

/*
 * Tell gcc if a function is cold. The compiler will assume any path
 * directly leading to the call is unlikely.
 */
#if GCC_VERSION >= 40300
# define __cold				__attribute__((__cold__))
# define __compiletime_warning(message)	__attribute__((warning(message)))
# define __compiletime_error(message)	__attribute__((error(message)))
#else
# define __cold
# define __compiletime_warning(message)
# define __compiletime_error(message)
#endif

/*
 * Mark a position in code as unreachable.  This can be used to
 * suppress control flow warnings after asm blocks that transfer
 * control elsewhere.
 */
#if GCC_VERSION >= 40500
# define unreachable()		__builtin_unreachable()
#else
# define unreachable()		do { } while(1)
#endif

/*
 * When used with Link Time Optimization, gcc can optimize away C functions or
 * variables which are referenced only from assembly code.  __visible tells the
 * optimizer that something else uses this function or variable, thus preventing
 * this.
 */
#if GCC_VERSION >= 40600
# define __visible	__attribute__((externally_visible))
#else
# define __visible
#endif

/*
 * GCC 'asm goto' miscompiles certain code sequences:
 * http://gcc.gnu.org/bugzilla/show_bug.cgi?id=58670
 * (asm goto is automatically volatile - the naming reflects this.)
 */
#define asm_volatile_goto(x...)		\
	do {				\
		asm goto(x);		\
		asm ("");		\
	} while (0)

#define __compiler_offsetof(a, b)	__builtin_offsetof(a, b)

#if GCC_VERSION >= 40400
#define __HAVE_BUILTIN_BSWAP32__
#define __HAVE_BUILTIN_BSWAP64__
#endif
#if GCC_VERSION >= 40800
#define __HAVE_BUILTIN_BSWAP16__
#endif

#endif	/* GCC_VERSION >= 40000 */

/*
 * Prevent the compiler from merging or refetching reads or writes. The
 * compiler is also forbidden from reordering successive instances of
 * READ_ONCE, WRITE_ONCE and ACCESS_ONCE (see below), but only when the
 * compiler is aware of some particular ordering.  One way to make the
 * compiler aware of ordering is to put the two invocations of READ_ONCE,
 * WRITE_ONCE or ACCESS_ONCE() in different C statements.
 *
 * In contrast to ACCESS_ONCE these two macros will also work on aggregate
 * data types like structs or unions. If the size of the accessed data
 * type exceeds the word size of the machine (e.g., 32 bits or 64 bits)
 * READ_ONCE() and WRITE_ONCE()  will fall back to memcpy and print a
 * compile-time warning.
 *
 * Their two major use cases are: (1) Mediating communication between
 * process-level code and irq/NMI handlers, all running on the same CPU,
 * and (2) Ensuring that the compiler does not  fold, spindle, or otherwise
 * mutilate accesses that either do not require ordering or that interact
 * with an explicit memory barrier or atomic instruction that provides the
 * required ordering.
 *
 * If possible use READ_ONCE()/WRITE_ONCE() instead.
 */

#ifndef __ASSEMBLY__

ALWAYS_INLINE void __read_once_size(const volatile void *p, void *res, int size)
{
	switch (size) {
	case 1: *(__u8 *)res = *(volatile __u8 *)p; break;
	case 2: *(__u16 *)res = *(volatile __u16 *)p; break;
	case 4: *(__u32 *)res = *(volatile __u32 *)p; break;
	case 8: *(__u64 *)res = *(volatile __u64 *)p; break;
	default:
		barrier();
		__builtin_memcpy((void *)res, (const void *)p, size);
		barrier();
	}
}

ALWAYS_INLINE void __write_once_size(volatile void *p, void *res, int size)
{
	switch (size) {
	case 1: *(volatile __u8 *)p = *(__u8 *)res; break;
	case 2: *(volatile __u16 *)p = *(__u16 *)res; break;
	case 4: *(volatile __u32 *)p = *(__u32 *)res; break;
	case 8: *(volatile __u64 *)p = *(__u64 *)res; break;
	default:
		barrier();
		__builtin_memcpy((void *)p, (const void *)res, size);
		barrier();
	}
}

#define READ_ONCE(x)						\
({								\
	union {							\
		typeof(x) __val;				\
		char __c[1];					\
	} __u;							\
	__read_once_size(&(x), __u.__c, sizeof(x));		\
	__u.__val;						\
})

#define WRITE_ONCE(x, val)					\
({								\
	union {							\
		typeof(x) __val;				\
		char __c[1];					\
	} __u = { .__val = (val) };				\
	__write_once_size(&(x), __u.__c, sizeof(x));		\
	__u.__val;						\
})

#define ACCESS_ONCE(x)						\
({								\
	__maybe_unused typeof(x) __var = (__force typeof(x)) 0;	\
	*(volatile typeof(x) *)&(x);				\
})

#endif /* __ASSEMBLY__ */

#endif /* _SANDIX_COMPILER_H_ */
