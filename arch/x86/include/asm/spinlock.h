/*
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

#ifndef _ASM_X86_SPINLOCK_H_
#define _ASM_X86_SPINLOCK_H_

#ifndef _SANDIX_SPINLOCK_H_
#error Please include <sandix/spinlock.h>
#endif

typedef struct arch_spinlock {

} arch_spinlock_t;

static inline void arch_spin_lock(arch_spinlock_t *lock)
{

}

static inline int arch_spin_trylock(arch_spinlock_t *lock)
{
	
}

static inline void arch_spin_unlock(arch_spinlock_t *lock)
{

}

#endif /* _ASM_X86_SPINLOCK_H_ */
