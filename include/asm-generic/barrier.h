/*
 *	Generic Barrier Definitions
 *
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

#ifndef _ASM_GENETIC_BARRIER_H_
#define _ASM_GENETIC_BARRIER_H_

#include <sandix/compiler.h>

/*
 * Force strict CPU ordering.
 * And yes, this is required on UP too when we're talking to devices.
 *
 * Fall back to compiler barriers if nothing better is provided.
 */

#ifndef mb
#define mb()	barrier()
#endif

#ifndef rmb
#define rmb()	mb()
#endif

#ifndef wmb
#define wmb()	mb()
#endif

#ifndef dma_rmb
#define dma_rmb()	rmb()
#endif

#ifndef dma_wmb
#define dma_wmb()	wmb()
#endif

#ifndef read_barrier_depends
#define read_barrier_depends()		do { } while (0)
#endif

#ifndef __smp_mb
#define __smp_mb()	mb()
#endif

#ifndef __smp_rmb
#define __smp_rmb()	rmb()
#endif

#ifndef __smp_wmb
#define __smp_wmb()	wmb()
#endif

#ifndef __smp_read_barrier_depends
#define __smp_read_barrier_depends()	read_barrier_depends()
#endif

#ifdef CONFIG_SMP

#ifndef smp_mb
#define smp_mb()	__smp_mb()
#endif

#ifndef smp_rmb
#define smp_rmb()	__smp_rmb()
#endif

#ifndef smp_wmb
#define smp_wmb()	__smp_wmb()
#endif

#ifndef smp_read_barrier_depends
#define smp_read_barrier_depends()	__smp_read_barrier_depends()
#endif

#else	/* !CONFIG_SMP */

#ifndef smp_mb
#define smp_mb()	barrier()
#endif

#ifndef smp_rmb
#define smp_rmb()	barrier()
#endif

#ifndef smp_wmb
#define smp_wmb()	barrier()
#endif

#ifndef smp_read_barrier_depends
#define smp_read_barrier_depends()	do { } while (0)
#endif

#endif	/* CONFIG_SMP */

#ifndef __smp_store_mb
#define __smp_store_mb(var, value)  do { WRITE_ONCE(var, value); __smp_mb(); } while (0)
#endif

#ifndef __smp_mb__before_atomic
#define __smp_mb__before_atomic()	__smp_mb()
#endif

#ifndef __smp_mb__after_atomic
#define __smp_mb__after_atomic()	__smp_mb()
#endif

#ifndef __smp_store_release
#define __smp_store_release(p, v)					\
do {									\
	compiletime_assert_atomic_type(*p);				\
	__smp_mb();							\
	WRITE_ONCE(*p, v);						\
} while (0)
#endif

#ifndef __smp_load_acquire
#define __smp_load_acquire(p)						\
({									\
	typeof(*p) ___p1 = READ_ONCE(*p);				\
	compiletime_assert_atomic_type(*p);				\
	__smp_mb();							\
	___p1;								\
})
#endif

#ifdef CONFIG_SMP

#ifndef smp_store_mb
#define smp_store_mb(var, value)  __smp_store_mb(var, value)
#endif

#ifndef smp_mb__before_atomic
#define smp_mb__before_atomic()	__smp_mb__before_atomic()
#endif

#ifndef smp_mb__after_atomic
#define smp_mb__after_atomic()	__smp_mb__after_atomic()
#endif

#ifndef smp_store_release
#define smp_store_release(p, v) __smp_store_release(p, v)
#endif

#ifndef smp_load_acquire
#define smp_load_acquire(p) __smp_load_acquire(p)
#endif

#else	/* !CONFIG_SMP */

#ifndef smp_store_mb
#define smp_store_mb(var, value)  do { WRITE_ONCE(var, value); barrier(); } while (0)
#endif

#ifndef smp_mb__before_atomic
#define smp_mb__before_atomic()	barrier()
#endif

#ifndef smp_mb__after_atomic
#define smp_mb__after_atomic()	barrier()
#endif

#ifndef smp_store_release
#define smp_store_release(p, v)						\
do {									\
	compiletime_assert_atomic_type(*p);				\
	barrier();							\
	WRITE_ONCE(*p, v);						\
} while (0)
#endif

#ifndef smp_load_acquire
#define smp_load_acquire(p)						\
({									\
	typeof(*p) ___p1 = READ_ONCE(*p);				\
	compiletime_assert_atomic_type(*p);				\
	barrier();							\
	___p1;								\
})
#endif

#endif /* CONFIG_SMP */

/* Barriers for virtual machine guests when talking to an SMP host */
#define virt_mb() __smp_mb()
#define virt_rmb() __smp_rmb()
#define virt_wmb() __smp_wmb()
#define virt_read_barrier_depends() __smp_read_barrier_depends()
#define virt_store_mb(var, value) __smp_store_mb(var, value)
#define virt_mb__before_atomic() __smp_mb__before_atomic()
#define virt_mb__after_atomic()	__smp_mb__after_atomic()
#define virt_store_release(p, v) __smp_store_release(p, v)
#define virt_load_acquire(p) __smp_load_acquire(p)

#endif /* _ASM_GENETIC_BARRIER_H_ */