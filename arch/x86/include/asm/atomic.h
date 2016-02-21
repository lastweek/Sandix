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

#ifndef _ASM_X86_ATOMIC_H_
#define _ASM_X86_ATOMIC_H_

#ifndef _SANDIX_ATOMIC_H_
# error "Please do not include this file directly"
#endif

#include <sandix/types.h>
#include <sandix/compiler.h>

#include <asm/asm.h>
#include <asm/cmpxchg.h>

#define ATOMIC_INIT(i)	{ (i) }

/**
 * atomic_read - read atomic variable
 * @v: pointer of type atomic_t
 *
 * Atomically reads the value of @v.
 */
static __always_inline int atomic_read(const atomic_t *v)
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
static __always_inline void atomic_set(atomic_t *v, int i)
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
static __always_inline void atomic_add(int i, atomic_t *v)
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
static __always_inline void atomic_sub(int i, atomic_t *v)
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
static __always_inline void atomic_inc(atomic_t *v)
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
static __always_inline void atomic_dec(atomic_t *v)
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
static __always_inline int atomic_cmpxchg(atomic_t *v, int old, int new)
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
static __always_inline int atomic_xchg(atomic_t *v, int new)
{
	return xchg(&v->counter, new);
}

#endif /* _ASM_X86_ATOMIC_H_ */
