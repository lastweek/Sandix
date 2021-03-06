/*
 *	Copyright (C) 2015-2016 Yizhou Shan <shan13@purdue.edu>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 */

#ifndef _ASM_X86_IRQFLAGS_H_
#define _ASM_X86_IRQFLAGS_H_

#include <asm/processor-flags.h>

static inline unsigned long native_save_flags(void)
{
	unsigned long flags;

	/*
	 * "=rm" is safe here, because "pop" adjusts the stack before
	 * it evaluates its effective address -- this is part of the
	 * documented behavior of the "pop" instruction.
	 */
	asm volatile (
		"pushf; pop %0"
		: "=rm" (flags)
		: /* no input */
		: "memory"
	);
	return flags;
}

static inline void native_restore_flags(unsigned long flags)
{
	asm volatile (
		"push %0; popf"
		: /* no output */
		: "g" (flags)
		: "memory", "cc"
	);
}

static inline void native_irq_disable(void)
{
	asm volatile ("cli" ::: "memory");
}

static inline void native_irq_enable(void)
{
	asm volatile ("sti" ::: "memory");
}

static inline void native_safe_halt(void)
{
	asm volatile ("sti; hlt" ::: "memory");
}

static inline void native_halt(void)
{
	asm volatile ("hlt" ::: "memory");
}

/*
 * Again, we skip paravirt here.
 */

static inline unsigned long arch_local_save_flags(void)
{
	return native_save_flags();
}

static inline void arch_local_irq_restore(unsigned long flags)
{
	native_restore_flags(flags);
}

static inline void arch_local_irq_disable(void)
{
	native_irq_disable();
}

static inline void arch_local_irq_enable(void)
{
	native_irq_enable();
}

/*
 * Used in the idle loop; sti taks one instruction cycle
 * to complete.
 */
static inline void arch_safe_halt(void)
{
	native_safe_halt();
}

/*
 * Used when interrupts are already enabled or to
 * shutdown the processor.
 */
static inline void arch_halt(void)
{
	native_halt();
}

/* For spinlocks, etc. */
static inline unsigned long arch_local_irq_save(void)
{
	unsigned long flags = arch_local_save_flags();
	arch_local_irq_disable();
	return flags;
}

static inline int arch_irqs_disabled_flags(unsigned long flags)
{
	return !(flags & X86_EFLAGS_IF);
}

static inline int arch_irqs_disabled(void)
{
	unsigned long flags = arch_local_save_flags();

	return arch_irqs_disabled_flags(flags);
}

#endif /* _ASM_X86_IRQFLAGS_H_ */
