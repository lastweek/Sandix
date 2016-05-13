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

#ifndef _SANDIX_SPINLOCK_SMP_H_
#define _SANDIX_SPINLOCK_SMP_H_

#ifndef _SANDIX_SPINLOCK_H_
# error "Please do not include this file directly"
#endif

#include <asm/spinlock.h>

/*
 * SMP spinlock definitions
 *
 * TODO:
 *	spinlock debug code, should be kernel/locking/spinlock_debug.c
 */

typedef struct spinlock {
	arch_spinlock_t arch_lock;
#ifdef CONFIG_DEBUG_SPINLOCK
	int magic, owner_cpu;
	void *owner;
#endif
} spinlock_t;

#define SPINLOCK_MAGIC 0xdead4ead

#ifdef CONFIG_DEBUG_SPINLOCK
# define SPIN_DEBUG_INIT(lockname)		\
	.magic		= SPINLOCK_MAGIC,	\
	.owner_cpu	= -1,			\
	.owner		= (void *)(-1L)
#else
# define SPIN_DEBUG_INIT(lockname)
#endif

#define __SPIN_LOCK_INIT(lockname)		\
{						\
	.arch_lock = ARCH_SPIN_LOCK_UNLOCKED,	\
	SPIN_DEBUG_INIT(lockname)		\
}

#define __SPIN_LOCK_UNLOCKED(lockname)		\
	(spinlock_t) __SPIN_LOCK_INIT(lockname)

/*
 * SMP spinlock operations
 */

static __always_inline void __spin_lock(spinlock_t *lock)
{
	preempt_disable();
	arch_spin_lock(&lock->arch_lock);
}

static __always_inline void __spin_lock_bh(spinlock_t *lock)
{
	local_bh_disable();
	arch_spin_lock(&lock->arch_lock);
}

static __always_inline void __spin_lock_irq(spinlock_t *lock)
{
	local_irq_disable();
	preempt_disable();
	arch_spin_lock(&lock->arch_lock);
}

#define __spin_lock_irqsave(lock, flags)		\
	do {						\
		local_irq_save(flags);			\
		preempt_disable();			\
		arch_spin_lock(&(lock)->arch_lock);	\
	} while (0)

static __always_inline void __spin_unlock(spinlock_t *lock)
{
	arch_spin_unlock(&lock->arch_lock);
	preempt_enable();
}

static __always_inline void __spin_unlock_bh(spinlock_t *lock)
{
	arch_spin_unlock(&lock->arch_lock);
	local_bh_enable();
}

static __always_inline void __spin_unlock_irq(spinlock_t *lock)
{
	arch_spin_unlock(&lock->arch_lock);
	local_irq_enable();
	preempt_enable();
}

static __always_inline void __spin_unlock_irqrestore(spinlock_t *lock,  unsigned long flags)
{
	arch_spin_unlock(&lock->arch_lock);
	local_irq_restore(flags);
	preempt_enable();
}

static __always_inline int __spin_trylock(spinlock_t *lock)
{
	preempt_disable();
	if (arch_spin_trylock(&lock->arch_lock))
		return 1;
	preempt_enable();
	return 0;
}

static __always_inline int __spin_trylock_bh(spinlock_t *lock)
{
	local_bh_disable();
	if (arch_spin_trylock(&lock->arch_lock))
		return 1;
	local_bh_enable();
	return 0;
}

static __always_inline int __spin_trylock_irq(spinlock_t *lock)
{
	local_irq_disable();
	preempt_disable();
	if (arch_spin_trylock(&lock->arch_lock))
		return 1;
	local_irq_enable();
	preempt_enable();
	return 0;
}

#define __spin_trylock_irqsave(lock, flags)		\
({							\
	local_irq_save(flags);				\
	preempt_disable();				\
	arch_spin_trylock(&(lock)->arch_flags) ? 1 :	\
	({						\
		local_irq_restore(flags);		\
		preempt_enable();			\
		0;					\
	});						\
})

static __always_inline int __spin_is_locked(spinlock_t *lock)
{
	return arch_spin_is_locked(&lock->arch_lock);
}

static __always_inline int __spin_is_contented(spinlock_t *lock)
{
	return arch_spin_is_contented(&lock->arch_lock);
}

#endif /* _SANDIX_SPINLOCK_SMP_H_ */
