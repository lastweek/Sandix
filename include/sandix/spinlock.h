/*
 *	include/sandix/spinlock.h - Spin Lock
 *
 *	Copyright (C) 2015 Yizhou Shan <shanyizhou@ict.ac.cn>
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

#include <asm/spinlock.h>

typedef struct spinlock {
	struct arch_spinlock alock;
} spinlock_t;

static inline void spin_lock(spinlock_t *lock)
{
	arch_spin_lock(&lock->alock);
}

static inline int spin_trylock(spinlock_t *lock)
{
	return arch_spin_trylock(&lock->alock);
}

static inline void spin_unlock(spinlock_t *lock)
{
	arch_spin_unlock(&lock->alock);
}

#endif /* _SANDIX_SPINLOCK_H_ */
