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

/* The first task of Sandix */

#include <sandix/sched.h>
#include <sandix/export.h>
#include <sandix/compiler.h>

#define INIT_MM(mm) {						\
								\
}

#define INIT_TASK(task)						\
{								\
	.state		= 0,					\
	.stack		= &init_thread_info,			\
	.usage		= ATOMIC_INIT(2),			\
	.flags		= PF_KTHREAD,				\
	.prio		= MAX_PRIO-20,				\
	.static_prio	= MAX_PRIO-20,				\
	.normal_prio	= MAX_PRIO-20,				\
	.policy		= SCHED_NORMAL,				\
	.nr_cpus_allowed= NR_CPUS,				\
	.tasks		= LIST_HEAD_INIT(task.tasks),		\
	.mm		= NULL,					\
	.active_mm	= &init_mm,				\
	.alloc_lock	= __SPIN_LOCK_UNLOCKED(task.alloc_lock),\
	.real_parent	= &task,				\
	.parent		= &task,				\
	.children	= LIST_HEAD_INIT(task.children),	\
	.sibling	= LIST_HEAD_INIT(task.sibling),		\
	.group_leader	= &task,				\
	.thread		= INIT_THREAD,				\
}

struct mm_struct   init_mm		= INIT_MM(init_mm);

struct task_struct init_task		= INIT_TASK(init_task);

/* Align init thread to THREAD_SIZE */
union thread_union init_thread_union __init_task_data = {
	INIT_THREAD_INFO(init_task) 
};

EXPORT_SYMBOL(init_task);
