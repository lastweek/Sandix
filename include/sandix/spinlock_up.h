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

/*
 * This file descibes UniProcessor version spinlock
 */

#ifndef _SANDIX_SPINLOCK_UP_H_
#define _SANDIX_SPINLOCK_UP_H_

#ifndef _SANDIX_SPINLOCK_H_
# error "Please do not include this file directly"
#endif

typedef struct {
	/* nothing */
} arch_spinlock_t;

#define arch_spin_lock(lock)
#define arch_spin_trylock(lock)
#define arch_spin_unlock(lock)

#define __spin_lock(lock)
#define __spin_trylock(lock)
#define __spin_unlock(lock)

#endif /* _SANDIX_SPINLOCK_UP_H_ */
