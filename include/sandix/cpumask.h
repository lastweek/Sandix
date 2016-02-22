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

#include <sandix/types.h>
#include <sandix/threads.h>
#include <sandix/bitmaps.h>

typedef struct cpumask {
	DEFINE_BITMAP(bits, NR_CPUS);
} cpumask_t;

#define cpumask_bits(maskp)	((maskp)->bits)

#if NR_CPUS == 1
#define nr_cpu_ids	1
#else
extern int nr_cpu_ids;
#endif

#define nr_cpumask_bits	nr_cpu_ids

#endif /* _SANDIX_CPUMASK_H_ */
