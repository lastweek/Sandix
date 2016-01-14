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

#ifndef _SANDIX_SPINLOCK_H_
#define _SANDIX_SPINLOCK_H_

#include <sandix/compiler.h>
#include <sandix/typecheck.h>
#include <sandix/irqflags.h>
#include <sandix/preempt.h>
#include <sandix/bottom_half.h>

#ifdef CONFIG_SMP
# include <sandix/spinlock_smp.h>
#else
# include <sandix/spinlock_up.h>
#endif

/*
 * Generic spinlock operations and definitions. These operations
 * are defined despite CONFIG_SMP and CONFIG_PREEMPT. They behave
 * as nops if not configured.
 */

#define DEFINE_SPINLOCK(lock)				\
	spinlock_t lock = __SPIN_LOCK_UNLOCKED(lock)

#define spin_lock_init(lock)				\
	do {						\
		*(lock) = __SPIN_LOCK_UNLOCKED((lock));	\
	} while (0)
	
static __always_inline void spin_lock(spinlock_t *lock)
{
	__spin_lock(lock);
}

static __always_inline void spin_lock_bh(spinlock_t *lock)
{
	__spin_lock_bh(lock);
}

static __always_inline void spin_lock_irq(spinlock_t *lock)
{
	__spin_lock_irq(lock);
}

#define spin_lock_irqsave(lock, flags)			\
	do {						\
		typecheck(unsigned long, flags);	\
		__spin_lock_irqsave((lock), flags);	\
	} while (0)

static __always_inline void spin_unlock(spinlock_t *lock)
{
	__spin_unlock(lock);
}

static __always_inline void spin_unlock_bh(spinlock_t *lock)
{
	__spin_unlock_bh(lock);
}

static __always_inline void spin_unlock_irq(spinlock_t *lock)
{
	__spin_unlock_irq(lock);
}

static __always_inline void spin_unlock_irqrestore(spinlock_t *lock,
						unsigned long flags)
{
	__spin_unlock_irqrestore(lock, flags);
}

static __always_inline int spin_trylock(spinlock_t *lock)
{
	return __spin_trylock(lock);
}

static __always_inline int spin_trylock_bh(spinlock_t *lock)
{
	return __spin_trylock_bh(lock);
}

static __always_inline int spin_trylock_irq(spinlock_t *lock)
{
	return __spin_trylock_irq(lock);
}

#define spin_trylock_irqsave(lock, flags)		\
	({						\
		typecheck(unsigned long, flags);	\
		__spin_trylock_irqsave((lock), flags);	\
	})	

static __always_inline int spin_is_locked(spinlock_t *lock)
{
	return __spin_is_locked(lock);
}

static __always_inline int spin_is_contented(spinlock_t *lock)
{
	return __spin_is_contented(lock);
}

#endif /* _SANDIX_SPINLOCK_H_ */
