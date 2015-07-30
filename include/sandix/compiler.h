#ifndef _SANDIX_COMPILER_H_
#define _SANDIX_COMPILER_H_

#include <asm/cache.h>

/*
 * Some common compiler friendly macros.
 * These macros are useless without dedicated linker scripits.
 */

#define __section(S)		__attribute__((section(#S)))

#define __init			__section(.init.text)
#define __initdata		__section(.init.data)
#define __iomem

#define __always_inline		inline __attribute__((always_inline))

#define INLINE			static inline
#define ALWAYS_INLINE		static __always_inline

#define EXPORT_SYMBOL(sym)	extern typeof(sym) sym;

#define likely(x)		__builtin_expect(!!(x), 1)
#define unlikely(x)		__builtin_expect(!!(x), 0)

#define GCC_VERSION (__GNUC__ * 10000 \
		   + __GNUC_MINOR__ * 100 \
		   + __GNUC_PATCHLEVEL__)

/*
 * This version is i.e. to prevent dead stores elimination on @ptr
 * where gcc and llvm may behave differently when otherwise using
 * normal barrier(): while gcc behavior gets along with a normal
 * barrier(), llvm needs an explicit input variable to be assumed
 * clobbered. The issue is as follows: while the inline asm might
 * access any memory it wants, the compiler could have fit all of
 * @ptr into memory registers instead, and since @ptr never escaped
 * from that, it proofed that the inline asm wasn't touching any of
 * it. This version works well with both compilers, i.e. we're telling
 * the compiler that the inline asm absolutely may see the contents
 * of @ptr. See also: https://llvm.org/bugs/show_bug.cgi?id=15495
 */
#define barrier_data(ptr) \
	asm volatile("": :"r"(ptr) :"memory")

#define barrier() \
	asm volatile("": : :"memory")

#endif /* _SANDIX_COMPILER_H_ */
