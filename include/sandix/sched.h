#ifndef _SANDIX_SCHED_H_
#define _SANDIX_SCHED_H_

#include <asm/processor.h>
#include <sandix/thread_info.h>
#include <sandix/mm.h>

#define TASK_RUNNING			0
#define TASK_INTERRUPTIBLE		1
#define TASK_UNINTERRRUPTIBLE	2

struct task_struct {
	volatile int state;	/* -1 unrunnable, 0 runnable, >0 stopped */
	void *stack;

	struct list_head tasks;
	
	pid_t	pid;							/* thread id */
	pid_t	tgid;							/* thread group id*/

	/*
	 * pointers to (original) parent process, youngest child, younger sibling,
	 * older sibling, respectively.  (p->father can be replaced with
	 * p->real_parent->pid)
	 */
	struct task_struct __rcu *real_parent;	/* real parent process */
	struct task_struct __rcu *parent;		/* recipient of SIGCHLD, wait4() reports */
	struct list_head children;				/* list of my children */
	struct list_head sibling;				/* list of my parent's children */
	
	struct task_struct	*group_leader;		/* threadgroup leader */
	struct list_head	thread_group;		/* threads in the same group */

	struct thread_struct thread;			/* cpu specific state of this task*/
	
	struct mm_struct *mm;					/* memory management struct */
}

#endif /* _SANDIX_SCHED_H_ */
