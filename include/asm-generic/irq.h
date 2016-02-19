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

#ifndef _ASM_GENERIC_IRQ_H_
#define _ASM_GENERIC_IRQ_H_

/*
 * NR_IRQS is the upper bound of how many interrupts can be handled
 * in the platform. It is used to size the static irq_map array,
 * so don't make it too big.
 */
#ifndef NR_IRQS
#define NR_IRQS 64
#endif

static inline int irq_canonicalize(int irq)
{
	return irq;
}

#endif /* _ASM_GENERIC_IRQ_H_ */
