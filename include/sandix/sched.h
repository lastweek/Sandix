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

#ifndef _SANDIX_SCHED_H_
#define _SANDIX_SCHED_H_

#include <asm/processor.h>

#include <sandix/mm.h>
#include <sandix/list.h>
#include <sandix/page.h>
#include <sandix/types.h>
#include <sandix/thread_info.h>

#define TASK_RUNNING		0
#define TASK_INTERRUPTIBLE	1
#define TASK_UNINTERRRUPTIBLE	2
#define TASK_STOPPED		3
#define TASK_TRACED		4

/* exit status */
#define EXIT_ZOMBIE		5
#define EXIT_DEAD		6

struct task_struct {
	volatile long state;		/* -1 unrunnable, 0 runnable, >0 stopped */
	void *stack;			/* kernel-mode stack */
	int priority;			/* schedule priority */

	struct list_head run_list;
	struct list_head tasks;
	
	/*
	 * The tgid is the pid of the first thread in threadgroup.
	 * Thus the first thread in group has the pid equals to tgid.
	 */
	pid_t	pid;			/* process or thread id */
	pid_t	tgid;			/* thread group id*/

	struct task_struct *group_leader;	/* threadgroup leader */
	struct list_head thread_group;		/* threads in the same group */

	struct task_struct *real_parent;	/* real original parent process */
	struct task_struct *parent;		/* recipient of SIGCHLD, wait4() reports */
	
	struct list_head children;		/* list of my children */
	struct list_head sibling;		/* list of my parent's children */
	
	struct thread_struct thread;		/* cpu specific state of this task*/
	
	struct mm_struct *mm;			/* memory management struct */
};

#define CURRENT_MASK		0xFFFFE000
#include <asm/current.h>

union thread_union {
	struct thread_info thread_info;
	unsigned int stack[THREAD_SIZE/sizeof(int)];
};

#define task_thread_info(task)	((struct thread_info *)(task)->stack)
#define task_stack_page(task)	((task)->stack)

#define next_task(p)	\
	list_entry((p)->tasks.next, struct task, tasks)

#define for_each_process(p)	\
	for (p = &init_task; (p = next_task(p)) != &init_task; )

void __init trap_init(void);

#endif /* _SANDIX_SCHED_H_ */
