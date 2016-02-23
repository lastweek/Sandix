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

#include <sandix/smp.h>
#include <sandix/tty.h>
#include <sandix/cpu.h>
#include <sandix/sched.h>
#include <sandix/types.h>
#include <sandix/kernel.h>
#include <sandix/printk.h>
#include <sandix/irqflags.h>

#include <asm/traps.h>
#include <asm/setup.h>
#include <asm/descriptor.h>
#include <asm/processor.h>

static void __init boot_cpu_init(void)
{
	int cpu = smp_processor_id();

	set_cpu_possible(cpu, true);
	set_cpu_present(cpu, true);
	set_cpu_online(cpu, true);
	set_cpu_active(cpu, true);
}

asmlinkage void __init start_kernel(void)
{
	local_irq_disable();

	/*
	 * We need printk. So, before arch_setup or boot_mem
	 * are initialized, let us just init tty first.
	 */
	early_arch_setup();
	tty_init();
	pr_notice("%s", sandix_banner);

	set_task_stack_end_magic(&init_task);

	boot_cpu_init();

	arch_setup();

	trap_init();

	asm (
		"movl 0xb0001000, %eax\n\t"	/* #PF */
		"movl $0x1, %eax\n\t"
		"int $0x80"
	);

	panic("init end");
}
