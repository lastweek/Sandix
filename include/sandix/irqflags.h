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
 * This file describes local_irq/flags_xxx APIs.
 */

#ifndef _SANDIX_IRQFLAGS_H_
#define _SANDIX_IRQFLAGS_H_

#include <asm/irqflags.h>
#include <sandix/typecheck.h>

static __always_inline void local_irq_enable(void)
{
	arch_local_irq_enable();
}

static inline void local_irq_disable(void)
{
	arch_local_irq_disable();
}

static inline void safe_halt(void)
{
	arch_safe_halt();
}

static inline void halt(void)
{
	arch_halt();
}

#define local_irq_save(flags)				\
	do {						\
		typecheck(unsigned long, flags);	\
		flags = arch_local_irq_save();		\
	} while (0)

#define local_irq_restore(flags)			\
	do {						\
		typecheck(unsigned long, flags);	\
		arch_local_irq_restore(flags);		\
	} while (0)

#define local_save_flags(flags)				\
	do {						\
		typecheck(unsigned long, flags);	\
		flags = arch_local_save_flags();	\
	} while (0)

#endif /* _SANDIX_IRQFLAGS_H_ */
