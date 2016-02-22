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

#include <asm/descriptor.h>

#include <sandix/sched.h>
#include <sandix/types.h>
#include <sandix/kernel.h>
#include <sandix/ptrace.h>
#include <sandix/compiler.h>

void __show_regs(struct pt_regs *regs, int all)
{
	unsigned long sp;
	unsigned short ss, gs;

	if (user_mode(regs)) {
		sp = regs->sp;
		ss = regs->ss & 0xffff;
		gs = regs->gs & 0xffff;
	} else {
		sp = regs->sp;
		//sp = kernel_stack_pointer(regs);
		savesegment(ss, ss);
		savesegment(gs, gs);
	}

	printk(KERN_DEFAULT "EIP: %04x:[<%08lx>] EFLAGS: %08lx\n",
		(u16)regs->cs, regs->ip, regs->flags);
	printk(KERN_DEFAULT "EAX: %08lx EBX: %08lx ECX: %08lx EDX: %08lx\n",
		regs->ax, regs->bx, regs->cx, regs->dx);
	printk(KERN_DEFAULT "ESI: %08lx EDI: %08lx EBP: %08lx ESP: %08lx\n",
		regs->si, regs->di, regs->bp, sp);
	printk(KERN_DEFAULT " DS: %04x ES: %04x FS: %04x GS: %04x SS: %04x\n",
	       (u16)regs->ds, (u16)regs->es, (u16)regs->fs, gs, ss);

	if (!all)
		return;
}

__visible struct task_struct *__switch_to(struct task_struct *prev,
					  struct task_struct *next)
{
}
