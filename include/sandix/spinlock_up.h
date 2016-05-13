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

#ifndef _SANDIX_SPINLOCK_UP_H_
#define _SANDIX_SPINLOCK_UP_H_

#ifndef _SANDIX_SPINLOCK_H_
# error "Please do not include this file directly"
#endif

/*
 * UP spinlock definitions, no debug version
 * if (CONFIG_SMP == no) --> CONFIG_DEBUG_SPINLOCK = no
 */

typedef struct { } arch_spinlock_t;
#define ARCH_SPIN_LOCK_UNLOCKED { }
#define arch_spin_lock(lock)		do { barrier(); (void)(lock); } while (0)
#define arch_spin_unlock(lock)		do { barrier(); (void)(lock); } while (0)
#define arch_spin_trylock(lock)		({ barrier(); (void)(lock); 1; })
#define arch_spin_is_locked(lock)	((void)(lock), 0)
#define arch_spin_is_contented(lock)	((void)(lock), 0)

typedef struct spinlock {
	arch_spinlock_t arch_lock;
} spinlock_t;

#define __SPIN_LOCK_INIT(lockname)		\
{						\
	.arch_lock = ARCH_SPIN_LOCK_UNLOCKED	\
}

#define __SPIN_LOCK_UNLOCKED(lockname)		\
	(spinlock_t) __SPIN_LOCK_INIT(lockname)

/*
 * In the UP-nondebug case there's no real locking going on, so the
 * only thing we have to do is to keep the preempt counts and irq
 * flags straight, to suppress compiler warnings of unused lock
 * variables, and to add the proper checker annotations:
 */
#define ___LOCK(lock) \
  do { (void)(lock); } while (0)

#define __LOCK(lock) \
  do { preempt_disable(); ___LOCK(lock); } while (0)

#define __LOCK_BH(lock) \
  do { local_bh_disable(); ___LOCK(lock); } while (0)

#define __LOCK_IRQ(lock) \
  do { local_irq_disable(); __LOCK(lock); } while (0)

#define __LOCK_IRQSAVE(lock, flags) \
  do { local_irq_save(flags); __LOCK(lock); } while (0)

#define ___UNLOCK(lock) \
  do { (void)(lock); } while (0)

#define __UNLOCK(lock) \
  do { preempt_enable(); ___UNLOCK(lock); } while (0)

#define __UNLOCK_BH(lock) \
  do { local_bh_enable(); ___UNLOCK(lock); } while (0)

#define __UNLOCK_IRQ(lock) \
  do { local_irq_enable(); __UNLOCK(lock); } while (0)

#define __UNLOCK_IRQRESTORE(lock, flags) \
  do { local_irq_restore(flags); __UNLOCK(lock); } while (0)

#define __spin_lock(lock)			__LOCK(lock)
#define __spin_lock_bh(lock)			__LOCK_BH(lock)
#define __spin_lock_irq(lock)			__LOCK_IRQ(lock)
#define __spin_lock_irqsave(lock, flags)	__LOCK_IRQSAVE(lock, flags)

#define __spin_unlock(lock)			__UNLOCK(lock)
#define __spin_unlock_bh(lock)			__UNLOCK_BH(lock)
#define __spin_unlock_irq(lock)			__UNLOCK_IRQ(lock)
#define __spin_unlock_irqrestore(lock, flags)	__UNLOCK_IRQRESTORE(lock, flags)

#define __spin_trylock(lock)			({ __LOCK(lock); 1; })
#define __spin_trylock_bh(lock)			({ __LOCK_BH(lock); 1; })
#define __spin_trylock_irq(lock)		({ __LOCK_IRQ(lock); 1; })
#define __spin_trylock_irqsave(lock, flags)	({ __LOCK_IRQSAVE(lock, flags); 1; })

#define __spin_is_locked(lock)			arch_spin_is_locked(lock)
#define __spin_is_contented(lock)		arch_spin_is_contented(lock)

#endif /* _SANDIX_SPINLOCK_UP_H_ */
