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

#include <sandix/sched.h>
#include <sandix/types.h>
#include <sandix/kernel.h>
#include <sandix/kdebug.h>
#include <sandix/ptrace.h>
#include <sandix/thread_info.h>

#include <asm/processor.h>

__visible void do_syscall_32_irqs_on(struct pt_regs *regs)
{
	struct thread_info *ti = pt_regs_to_thread_info(regs);

	printk("SYSCALL baby...\n");
	show_regs(regs);
}
