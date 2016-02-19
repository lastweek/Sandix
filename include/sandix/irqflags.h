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

#ifndef _SANDIX_IRQFLAGS_H_
#define _SANDIX_IRQFLAGS_H_

#include <asm/irqflags.h>
#include <sandix/typecheck.h>

#define local_irq_enable()	arch_local_irq_enable()
#define local_irq_disable()	arch_local_irq_disable()

#define halt()			arch_halt()
#define safe_halt()		arch_safe_halt()

/*
 * Test hardware interrupt enable bit
 */
#define irqs_disabled()		arch_irqs_disabled()

/*
 * Test flags
 */
#define irqs_disabled_flags(flags)			\
	do {						\
		typecheck(unsigned long, flags);	\
		arch_irqs_disabled_flags(flags);	\
	} while (0)

/*
 * Get status and disable interrupts
 */
#define local_irq_save(flags)				\
	do {						\
		typecheck(unsigned long, flags);	\
		flags = arch_local_irq_save();		\
	} while (0)

/*
 * Set interrupt enabled status
 */
#define local_irq_restore(flags)			\
	do {						\
		typecheck(unsigned long, flags);	\
		arch_local_irq_restore(flags);		\
	} while (0)

/*
 * Read interrupt enabled status
 */
#define local_save_flags(flags)				\
	do {						\
		typecheck(unsigned long, flags);	\
		flags = arch_local_save_flags();	\
	} while (0)

#endif /* _SANDIX_IRQFLAGS_H_ */
