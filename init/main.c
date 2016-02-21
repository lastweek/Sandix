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

#include <sandix/tty.h>
#include <sandix/sched.h>
#include <sandix/types.h>
#include <sandix/kernel.h>
#include <sandix/printk.h>

#include <asm/traps.h>
#include <asm/setup.h>
#include <asm/descriptor.h>

asmlinkage void __init start_kernel(void)
{
	/*
	 * We need printk to print some info. So, before arch_setup or boot_mem
	 * are built, let us just init tty first. Anyway, we need to rewrite some
	 * code within tty after malloc/free done.
	 */
	early_arch_setup();
	tty_init();
	tty_print_drivers();
	printk("\033[32m%s\033[0m\n\r", sandix_banner);

	arch_setup();
	trap_init();

	current_thread_info();

	asm (
		"movl 0xb0001000, %eax\n\t"	/* #PF */
		"movl $0x1, %eax\n\t"
		"int $0x80"
	);

	panic("init end");
}
