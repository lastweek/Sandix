/*
 *	Copyright (C) 2015-2016 Yizhou Shan <shan13@purdue.edu>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 */

#ifndef _SANDIX_IRQ_H_
#define _SANDIX_IRQ_H_

#include <sandix/compiler.h>

#include <asm/irq.h>

struct irqaction {
	const char	*name;
};

/**
 * struct irq_desc	-	Interrupt Descriptor
 */
struct irq_desc {

};

#define irq_disable()	arch_irq_disable()
#define irq_enable()	arch_irq_enable()

void __init IRQ_init(void);

#endif /* _SANDIX_IRQ_H_ */
