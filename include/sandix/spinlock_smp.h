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

#ifndef _SANDIX_SPINLOCK_SMP_H_
#define _SANDIX_SPINLOCK_SMP_H_

#ifndef _SANDIX_SPINLOCK_H_
# error "Please do not include this file directly"
#endif

#include <asm/spinlock.h>

/*
 * SMP spinlock definitions
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

#define SPIN_LOCK_INIT(lockname)		\
{						\
	.arch_lock = ARCH_SPIN_LOCK_UNLOCKED,	\
	SPIN_DEBUG_INIT(lockname)		\
}

#define SPIN_LOCK_UNLOCKED(lockname)		\
	(spinlock_t) SPIN_LOCK_INIT(lockname)

#define DEFINE_SPINLOCK(x)	spinlock_t x = SPIN_LOCK_UNLOCKED(x)

/*
 * SMP spinlock operations
 */

static __always_inline void __spin_lock(spinlock_t *lock)
{

}

static __always_inline void __spin_lock_bh(spinlock_t *lock)
{

}

static __always_inline void __spin_lock_irq(spinlock_t *lock)
{

}

static __always_inline unsigned long __spin_lock_irqsave(spinlock_t *lock)
{

}

static __always_inline int __spin_trylock(spinlock_t *lock)
{

}

static __always_inline int __spin_trylock_bh(spinlock_t *lock)
{

}

static __always_inline void __spin_unlock(spinlock_t *lock)
{

}

static __always_inline void __spin_unlock_bh(spinlock_t *lock)
{

}

static __always_inline void __spin_unlock_irq(spinlock_t *lock)
{

}

static __always_inline void __spin_unlock_irqrestore(spinlock_t *lock)
{

}

#endif /* _SANDIX_SPINLOCK_SMP_H_ */
