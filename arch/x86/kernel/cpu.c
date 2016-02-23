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

#include <asm/processor.h>

#include <sandix/types.h>
#include <sandix/kernel.h>
#include <sandix/printk.h>
#include <sandix/compiler.h>

static struct cpu_dev *cpudevs[] = {
	[X86_VENDOR_INTEL]	= &intel_cpu_dev,
	[X86_VENDOR_UNKNOWN]	= NULL
};

void __init early_cpu_init(void)
{
	int i = 0;

	while (cpudevs[i++]) {
		struct cpu_dev *cdev = cpudevs[i - 1];

		printk(KERN_INFO "%s\n", cdev->cpu_ident);
	}
}

void __init cpu_init(void)
{

}
