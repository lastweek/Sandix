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

#ifndef _ASM_X86_SPINLOCK_H_
#define _ASM_X86_SPINLOCK_H_

#ifndef _SANDIX_SPINLOCK_H_
# error "Please do not include this file directly"
#endif

#include <sandix/compiler.h>
#include <sandix/types.h>

#ifndef CONFIG_X86_SPINLOCK_TICKET
/*
 * Basic SMP spinlocks, this makes no fairness assumptions
 */

typedef struct arch_spinlock {
	unsigned int slock;
} arch_spinlock_t;

#define ARCH_SPIN_LOCK_UNLOCKED { 0 }

static __always_inline void arch_spin_lock(arch_spinlock_t *lock)
{
	asm volatile (
		
	);
}

static __always_inline int arch_spin_trylock(arch_spinlock_t *lock)
{
	asm volatile  (
	
	);
	return 0;
}

static __always_inline void arch_spin_unlock(arch_spinlock_t *lock)
{
	asm volatile (
	
	);
}

static __always_inline int arch_spin_is_locked(arch_spinlock_t *lock)
{
	return 0;
}

static __always_inline int arch_spin_is_contented(arch_spinlock_t *lock)
{
	return 0;
}

#else
/*
 * Ticket SMP spinlocks, this ensures fairness with little cost
 */

#if 1
typedef u8  __ticket_t;
typedef u16 __ticketpair_t;
#else
typedef u16  __ticket_t;
typedef u32 __ticketpair_t;
#endif

typedef struct arch_spinlock {
	union {
		__ticketpair_t head_tail;
		struct __raw_tickets {
			__ticket_t head, tail;
		} tickets;
	}
} arch_spinlock_t;

#define ARCH_SPIN_LOCK_UNLOCKED { { 0 } }

/*
 * Ticket locks are conceptually two parts, one indicating the current head of
 * the queue, and the other indicating the current tail. The lock is acquired
 * by atomically noting the tail and incrementing it by one (thus adding
 * ourself to the queue and noting our position), then waiting until the head
 * becomes equal to the the initial value of the tail.
 *
 * We use an xadd covering *both* parts of the lock, to increment the tail and
 * also load the position of the head, which takes care of memory ordering
 * issues and should be optimal for the uncontended case. Note the tail must be
 * in the high part, because a wide xadd increment of the low part would carry
 * up and contaminate the high part.
 */
static __always_inline void arch_spin_lock(arch_spinlock_t *lock)
{
	asm volatile (
		
	);
}

static __always_inline int arch_spin_trylock(arch_spinlock_t *lock)
{
	asm volatile  (
	
	);
	return 0;
}

static __always_inline void arch_spin_unlock(arch_spinlock_t *lock)
{
	asm volatile (
	
	);
}

static __always_inline int arch_spin_is_locked(arch_spinlock_t *lock)
{
	return 0;
}

static __always_inline int arch_spin_is_contented(arch_spinlock_t *lock)
{
	return 0;
}

#endif /* CONFIG_X86_SPINLOCK_TICKET */

#define arch_spin_relax(lock)	cpu_relax()
#define arch_read_relax(lock)	cpu_relax()
#define arch_write_relax(lock)	cpu_relax()

#endif /* _ASM_X86_SPINLOCK_H_ */
