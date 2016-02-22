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

#ifndef _SANDIX_CPUMASK_H_
#define _SANDIX_CPUMASK_H_

#include <sandix/bug.h>
#include <sandix/types.h>
#include <sandix/bitops.h>
#include <sandix/threads.h>
#include <sandix/bitmaps.h>

#if NR_CPUS == 1
#define nr_cpu_ids 1
#else
extern int nr_cpu_ids; /* kernel/smp.c */
#endif

typedef struct cpumask {
	DEFINE_BITMAP(bits, NR_CPUS);
} cpumask_t;

#define bits_to_cpumask(bits)	(struct cpumask *)(bits)
#define cpumask_to_bits(maskp)	((maskp)->bits)

extern const struct cpumask *const cpu_possible_mask;
extern const struct cpumask *const cpu_present_mask;
extern const struct cpumask *const cpu_online_mask;
extern const struct cpumask *const cpu_active_mask;

/* verify cpu argument to cpumask_* operators */
static inline unsigned int cpumask_check(unsigned int cpu)
{
	WARN_ON(cpu >= nr_cpu_ids);
	return cpu;
}

static inline void cpumask_set_cpu(unsigned int cpu, struct cpumask *maskp)
{
	set_bit(cpumask_check(cpu), cpumask_to_bits(maskp));
}

static inline void cpumask_clear_cpu(unsigned int cpu, struct cpumask *maskp)
{
	clear_bit(cpumask_check(cpu), cpumask_to_bits(maskp));
}

static inline int cpumask_test_cpu(unsigned int cpu, const struct cpumask *maskp)
{
	return test_bit(cpumask_check(cpu), cpumask_to_bits(maskp));
}

/* Wrappers for boot code to modify constant masks */
void set_cpu_possible(unsigned int cpu, bool possible);
void set_cpu_present(unsigned int cpu, bool present);
void set_cpu_online(unsigned int cpu, bool online);
void set_cpu_active(unsigned int cpu, bool active);

#endif /* _SANDIX_CPUMASK_H_ */
