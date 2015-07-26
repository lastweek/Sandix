#ifndef _SAM_ATOMIC_H_
#define _SAM_ATOMIC_H_

#ifndef _SANDIX_ATOMIC_H_
# error "Please do not include this file directly, include <sandix/atomic.h>"
#endif

#include <sandix/types.h>
#include <asm/asm.h>
#include <asm/cmpxchg.h>

#define ATOMIC_INIT(i)	{ (i) }

/**
 * atomic_read - read atomic variable
 * @v: pointer of type atomic_t
 *
 * Atomically reads the value of @v.
 */
static inline int atomic_read(const atomic_t *v)
{
	return v->counter;
}

/**
 * atomic_set - set atomic variable
 * @v: pointer of type atomic_t
 * @i: required value
 *
 * Atomically sets the value of @v to @i.
 */
static inline void atomic_set(atomic_t *v, int i)
{
	v->counter = i;
}

/**
 * atomic_add - add integer to atomic variable
 * @i: integer value to add
 * @v: pointer of type atomic_t
 *
 * Atomically adds @i to @v.
 */
static inline void atomic_add(int i, atomic_t *v)
{
	asm volatile (
		LOCK_PREFIX "addl %1, %0"
		: "+m" (v->counter)
		: "ir" (i)
	);
}

/**
 * atomic_sub - subtract integer from atomic variable
 * @i: integer value to subtract
 * @v: pointer of type atomic_t
 *
 * Atomically subtracts @i from @v.
 */
static inline void atomic_sub(int i, atomic_t *v)
{
	asm volatile (
		LOCK_PREFIX "subl %1, %0"
		: "+m" (v->counter)
		: "ir" (i)
	);
}

/**
 * atomic_inc - increment atomic variable
 * @v: pointer of type atomic_t
 *
 * Atomically increments @v by 1.
 */
static inline void atomic_inc(atomic_t *v)
{
	asm volatile (
		LOCK_PREFIX "incl %0"
		: "+m" (v->counter)
	);
}

/**
 * atomic_dec - decrement atomic variable
 * @v: pointer of type atomic_t
 *
 * Atomically decrements @v by 1.
 */
static inline void atomic_dec(atomic_t *v)
{
	asm volatile (
		LOCK_PREFIX "decl %0"
		: "+m" (v->counter)
	);
}

/**
 * atomic_cmpxchg - compare and exchange
 * @v:   pointer of type atomic_t
 * @old: old value to compare
 * @new: new value to store
 *
 * Atomically compare and exchange.
 */
static inline int atomic_cmpxchg(atomic_t *v, int old, int new)
{
	return cmpxchg(&v->counter, old, new);
}

/**
 * atomic_xchg - exchange two values
 * @v:   pointer of type atomic_t
 * @new: new value to store
 *
 * Atomically exchange.
 */
static inline int atomic_xchg(atomic_t *v, int new)
{
	return xchg(&v->counter, new);
}

#endif /* _SAM_ATOMIC_H_ */
