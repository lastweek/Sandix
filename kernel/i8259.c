/*
 *	kernel/i8259.c - Legacy Intel 8259A PIC Support
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
 *
 */

#include <sandix/compiler.h>
#include <sandix/types.h>
#include <sandix/irq.h>

/* i8259A PIC registers */
#define PIC_MASTER_CMD		0x20
#define PIC_MASTER_IMR		0x21
#define PIC_MASTER_ISR		PIC_MASTER_CMD
#define PIC_MASTER_POLL		PIC_MASTER_ISR
#define PIC_MASTER_OCW3		PIC_MASTER_ISR
#define PIC_SLAVE_CMD		0xa0
#define PIC_SLAVE_IMR		0xa1

static void mask_8259A_irq(unsigned int irq)
{

}

static void unmask_8259A_irq(unsigned int irq)
{

}

static void disable_8259A_irq(unsigned int irq)
{
	mask_8259A_irq(irq);
}

static void enable_8259A_irq(unsigned int irq)
{
	unmask_8259A_irq(irq);
}

void __init init_8259A(void)
{

}
