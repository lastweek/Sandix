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

#define __ARCH_SPIN_LOCK_UNLOCKED { 0 }

static __always_inline void arch_spin_lock(arch_spinlock_t *lock)
{
	asm volatile (
		
	);
}

static __always_inline int arch_spin_trylock(arch_spinlock_t *lock)
{
	asm volatile  (
	
	);
}

static __always_inline void arch_spin_unlock(arch_spinlock_t *lock)
{
	asm volatile (
	
	);
}

#elif
/*
 * Ticket SMP spinlocks, this ensures fairness with little cost
 */
#error "It is not implemented yet"

#endif /* CONFIG_X86_SPINLOCK_TICKET */

#endif /* _ASM_X86_SPINLOCK_H_ */
