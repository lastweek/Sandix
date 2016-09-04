/*
 *	Copyright (C) 2015-2016 Yizhou Shan <shan13@purdue.edu>
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

#ifndef _ASM_X86_APIC_H_
#define _ASM_X86_APIC_H_

#include <sandix/cpumask.h>

#define IO_APIC_DEFAULT_PHYS_BASE	0xfec00000
#define	APIC_DEFAULT_PHYS_BASE		0xfee00000

#ifdef CONFIG_X86_32
# define MAX_IO_APICS			64
# define MAX_LOCAL_APIC			256
#else
# define MAX_IO_APICS			128
# define MAX_LOCAL_APIC			32768
#endif

#if defined(CONFIG_X86_LOCAL_APIC) && defined(CONFIG_X86_32)
void generic_apic_probe(void);
#else
static inline void generic_apic_probe(void) { }
#endif

struct apic {
	char		*name;
	int		(*probe)(void);

	/* IPI */
	void		(*send_IPI)(int cpu, int vector);
	void		(*send_IPI_mask)(const struct cpumask *mask, int vector);
	void		(*send_IPI_allbutself)(int vector);
	void		(*send_IPI_all)(int vector);
	void		(*send_IPI_self)(int vector);
};

/*
 * Pointer to the local APIC driver in use on this system (there's
 * always just one such driver in use - the kernel decides via an
 * early probing process which one it picks - and then sticks to it):
 */
extern struct apic *apic;

/*
 * APIC drivers are probed based on how they are listed in the .apicdrivers
 * section. So the order is important and enforced by the ordering
 * of different apic driver files in the Makefile.
 *
 * For the files having two apic drivers, we use apic_drivers()
 * to enforce the order with in them.
 */
#define apic_driver(sym)						\
	static const struct apic *__apicdrivers_##sym __used		\
	__aligned(sizeof(struct apic *))				\
	__section(.apicdrivers) = { &sym }

#define apic_drivers(sym1, sym2)					\
	static struct apic *__apicdrivers_##sym1##sym2[2] __used	\
	__aligned(sizeof(struct apic *))				\
	__section(.apicdrivers) = { &sym1, &sym2 }

extern struct apic *__apicdrivers[], *__apicdrivers_end[];

void apic_init_mappings(void);
void io_apic_init_mappings(void);

#endif /* _ASM_X86_APIC_H_ */
