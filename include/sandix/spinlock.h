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

#ifdef CONFIG_SMP
# include <sandix/spinlock_smp.h>
#else
# include <sandix/spinlock_up.h>
#endif

/*
 * Generic spinlock operations
 */

/*
 * Use __always_inline because: [https://lkml.org/lkml/2015/7/21/206]
 */
static __always_inline void spin_lock(spinlock_t *lock)
{
	__spin_lock(lock);
}

static __always_inline int spin_trylock(spinlock_t *lock)
{
	return __spin_trylock(lock);
}

static __always_inline void spin_unlock(spinlock_t *lock)
{
	__spin_unlock(lock);
}

#endif /* _SANDIX_SPINLOCK_H_ */
