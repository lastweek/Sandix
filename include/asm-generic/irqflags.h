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

#ifndef _ASM_GENERIC_IRQFLAGS_H_
#define _ASM_GENERIC_IRQFLAGS_H_

/*
 * All architectures should implement at least the first two functions,
 * usually inline assembly will be the best way.
 */
#ifndef ARCH_IRQ_DISABLED
#define ARCH_IRQ_DISABLED 0
#define ARCH_IRQ_ENABLED 1
#endif

/* read interrupt enabled status */
#ifndef arch_local_save_flags
unsigned long arch_local_save_flags(void);
#endif

/* set interrupt enabled status */
#ifndef arch_local_irq_restore
void arch_local_irq_restore(unsigned long flags);
#endif

/* get status and disable interrupts */
#ifndef arch_local_irq_save
static inline unsigned long arch_local_irq_save(void)
{
	unsigned long flags;
	flags = arch_local_save_flags();
	arch_local_irq_restore(ARCH_IRQ_DISABLED);
	return flags;
}
#endif

/* test flags */
#ifndef arch_irqs_disabled_flags
static inline int arch_irqs_disabled_flags(unsigned long flags)
{
	return flags == ARCH_IRQ_DISABLED;
}
#endif

/* unconditionally enable interrupts */
#ifndef arch_local_irq_enable
static inline void arch_local_irq_enable(void)
{
	arch_local_irq_restore(ARCH_IRQ_ENABLED);
}
#endif

/* unconditionally disable interrupts */
#ifndef arch_local_irq_disable
static inline void arch_local_irq_disable(void)
{
	arch_local_irq_restore(ARCH_IRQ_DISABLED);
}
#endif

/* test hardware interrupt enable bit */
#ifndef arch_irqs_disabled
static inline int arch_irqs_disabled(void)
{
	return arch_irqs_disabled_flags(arch_local_save_flags());
}
#endif

#endif /* _ASM_GENERIC_IRQFLAGS_H_ */
