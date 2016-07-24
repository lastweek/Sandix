/*
 *	Copyright (C) 2015-2016 Yizhou Shan <shan13@purdue.edu>
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

#ifndef _ASM_X86_BITOPS_H_
#define _ASM_X86_BITOPS_H_

#ifndef _SANDIX_BITOPS_H_
# error "Please do not include this file directly"
#endif

#include <asm/asm.h>
#include <sandix/types.h>
#include <sandix/compiler.h>

#if BITS_PER_LONG == 32
# define __BITOPS_LONG_SHIFT 5
#elif BITS_PER_LONG == 64
# define __BITOPS_LONG_SHIFT 6
#else
# error "Unexpected BITS_PER_LONG"
#endif

/*
 * These have to be done with inline assembly: that way the bit-setting
 * is guaranteed to be atomic. All bit operations return 0 if the bit
 * was cleared before the operation and != 0 if it was not.
 *
 * bit 0 is the LSB of addr; bit 32 is the LSB of (addr+1).
 */

#define ADDR (*(volatile long *) addr)

/**
 * set_bit - Atomically set a bit in memory
 * @nr: the bit to set
 * @addr: the address to start counting from
 *
 * This function is atomic and may not be reordered.  See __set_bit()
 * if you do not require the atomic guarantees.
 *
 * Note: there are no guarantees that this function will not be reordered
 * on non x86 architectures, so if you are writing portable code,
 * make sure not to rely on its reordering guarantees.
 *
 * Note that @nr may be almost arbitrarily large; this function is not
 * restricted to acting on a single-word quantity.
 */
static __always_inline void set_bit(int nr, volatile unsigned long * addr)
{
	asm volatile (
		LOCK_PREFIX "btsl %1,%0"
		: "+m" (ADDR)
		: "Ir" (nr)
		: "memory"
	);
}

/**
 * __set_bit - Set a bit in memory
 * @nr: the bit to set
 * @addr: the address to start counting from
 *
 * Unlike set_bit(), this function is non-atomic and may be reordered.
 * If it's called on the same region of memory simultaneously, the effect
 * may be that only one operation succeeds.
 */
static __always_inline void __set_bit(long nr, volatile unsigned long *addr)
{
	asm volatile (
		"bts %1,%0"
		: "+m" ADDR
		: "Ir" (nr)
		: "memory"
	);
}

/**
 * clear_bit - Clears a bit in memory
 * @nr: Bit to clear
 * @addr: Address to start counting from
 *
 * clear_bit() is atomic and may not be reordered.  However, it does
 * not contain a memory barrier, so if it is used for locking purposes,
 * you should call smp_mb__before_clear_bit() and/or smp_mb__after_clear_bit()
 * in order to ensure changes are visible on other processors.
 */
static inline void clear_bit(int nr, volatile unsigned long * addr)
{
	asm volatile (
		LOCK_PREFIX "btrl %1,%0"
		:"+m" (ADDR)
		:"Ir" (nr)
	);
}

/*
 * clear_bit_unlock - Clears a bit in memory
 * @nr: Bit to clear
 * @addr: Address to start counting from
 *
 * clear_bit() is atomic and implies release semantics before the memory
 * operation. It can be used for an unlock.
 */
static __always_inline void clear_bit_unlock(long nr, volatile unsigned long *addr)
{
	barrier();
	clear_bit(nr, addr);
}

static __always_inline void __clear_bit(long nr, volatile unsigned long *addr)
{
	asm volatile (
		"btr %1,%0"
		: "+m" ADDR
		: "Ir" (nr)
	);
}

/*
 * __clear_bit_unlock - Clears a bit in memory
 * @nr: Bit to clear
 * @addr: Address to start counting from
 *
 * __clear_bit() is non-atomic and implies release semantics before the memory
 * operation. It can be used for an unlock if no other CPUs can concurrently
 * modify other bits in the word.
 *
 * No memory barrier is required here, because x86 cannot reorder stores past
 * older loads. Same principle as spin_unlock.
 */
static __always_inline void __clear_bit_unlock(long nr, volatile unsigned long *addr)
{
	barrier();
	__clear_bit(nr, addr);
}

/**
 * test_and_clear_bit - Clear a bit and return its old value
 * @nr: Bit to clear
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.
 * It can be reorderdered on other architectures other than x86.
 * It also implies a memory barrier.
 */
static inline int test_and_clear_bit(int nr, volatile unsigned long * addr)
{
	int oldbit;

	asm volatile( LOCK_PREFIX
		"btrl %2,%1\n\tsbbl %0,%0"
		:"=r" (oldbit),"+m" (ADDR)
		:"Ir" (nr) : "memory");
	return oldbit;
}

/**
 * __test_and_clear_bit - Clear a bit and return its old value
 * @nr: Bit to clear
 * @addr: Address to count from
 *
 * This operation is non-atomic and can be reordered.  
 * If two examples of this operation race, one can appear to succeed
 * but actually fail.  You must protect multiple accesses with a lock.
 */
static inline int __test_and_clear_bit(int nr, volatile unsigned long *addr)
{
	int oldbit;

	asm (
		"btrl %2,%1\n\tsbbl %0,%0"
		:"=r" (oldbit),"+m" (ADDR)
		:"Ir" (nr));
	return oldbit;
}

/**
 * test_and_set_bit - Set a bit and return its old value
 * @nr: Bit to set
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.  
 * It may be reordered on other architectures than x86.
 * It also implies a memory barrier.
 */
static inline int test_and_set_bit(int nr, volatile unsigned long * addr)
{
	int oldbit;

	asm volatile (
		LOCK_PREFIX "btsl %2,%1\n\tsbbl %0,%0"
		:"=r" (oldbit),"+m" (ADDR)
		:"Ir" (nr) : "memory"
	);
	return oldbit;
}

static __always_inline int constant_test_bit(int nr, const volatile unsigned long *addr)
{
	return ((1UL << (nr & 31)) & (addr[nr >> 5])) != 0;
}

static inline int variable_test_bit(int nr, const volatile unsigned long * addr)
{
	int oldbit;

	asm volatile (
		"btl %2,%1\n\tsbbl %0,%0"
		:"=r" (oldbit)
		:"m" (ADDR),"Ir" (nr)
	);
	return oldbit;
}

#define test_bit(nr,addr)			\
(						\
	__builtin_constant_p(nr) ?		\
	constant_test_bit((nr),(addr)) :	\
	variable_test_bit((nr),(addr))		\
)

#undef ADDR

#endif /* _ASM_X86_BITOPS_H_ */
