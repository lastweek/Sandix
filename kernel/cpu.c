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

#include <sandix/cpu.h>
#include <sandix/types.h>
#include <sandix/export.h>
#include <sandix/threads.h>
#include <sandix/compiler.h>

static DEFINE_BITMAP(cpu_possible_bits, NR_CPUS) __read_mostly;
static DEFINE_BITMAP(cpu_present_bits, NR_CPUS) __read_mostly;
static DEFINE_BITMAP(cpu_online_bits, NR_CPUS) __read_mostly;
static DEFINE_BITMAP(cpu_active_bits, NR_CPUS) __read_mostly;

const struct cpumask *const cpu_possible_mask = bits_to_cpumask(cpu_possible_bits);
const struct cpumask *const cpu_present_mask = bits_to_cpumask(cpu_present_bits);
const struct cpumask *const cpu_online_mask = bits_to_cpumask(cpu_online_bits);
const struct cpumask *const cpu_active_mask = bits_to_cpumask(cpu_active_bits);

EXPORT_SYMBOL(cpu_possible_mask);
EXPORT_SYMBOL(cpu_present_mask);
EXPORT_SYMBOL(cpu_online_mask);
EXPORT_SYMBOL(cpu_active_mask);

void set_cpu_possible(unsigned int cpu, bool possible)
{
	if (possible)
		cpumask_set_cpu(cpu, bits_to_cpumask(cpu_possible_bits));
	else
		cpumask_clear_cpu(cpu, bits_to_cpumask(cpu_possible_bits));
}

void set_cpu_present(unsigned int cpu, bool present)
{
	if (present)
		cpumask_set_cpu(cpu, bits_to_cpumask(cpu_present_bits));
	else
		cpumask_clear_cpu(cpu, bits_to_cpumask(cpu_present_bits));
}

void set_cpu_online(unsigned int cpu, bool online)
{
	if (online) {
		cpumask_set_cpu(cpu, bits_to_cpumask(cpu_online_bits));
		cpumask_set_cpu(cpu, bits_to_cpumask(cpu_active_bits));
	} else {
		cpumask_clear_cpu(cpu, bits_to_cpumask(cpu_online_bits));
	}
}

void set_cpu_active(unsigned int cpu, bool active)
{
	if (active)
		cpumask_set_cpu(cpu, bits_to_cpumask(cpu_active_bits));
	else
		cpumask_clear_cpu(cpu, bits_to_cpumask(cpu_active_bits));
}
