#ifndef _SANDIX_COMPILER_H_
#define _SANDIX_COMPILER_H_

#define GCC_VERSION (__GNUC__ * 10000 \
		   + __GNUC_MINOR__ * 100 \
		   + __GNUC_PATCHLEVEL__)


/* Many architecture use 64 bytes cache line */
#define L1_CACHE_LINE_SIZE	64

#define __section(S)		__attribute__((__section__(#S)))

/*
 * Normally, if one byte in a cache line is diry, then the entire
 * cache line will need to be flushed back. Put the read most, which also,
 * write rarely data together, then a lot of useless cache line write
 * events can be avoided.
 */
#define __read_mostly		__section(.data..read_mostly)
#define __init			__section(.init.text)
#define __initdata		__section(.init.data)

#define __user
#define __iomem

#define __always_inline		inline __attribute__((always_inline))
#define INLINE			static inline
#define ALWAYS_INLINE		static __always_inline

#define EXPORT_SYMBOL(sym)	extern typeof(sym) sym;

#define likely(x)		__builtin_expect(!!(x), 1)
#define unlikely(x)		__builtin_expect(!!(x), 0)

#define barrier()		asm volatile("": : :"memory")

#endif /* _SANDIX_COMPILER_H_ */
