#ifndef _SANDIX_SCHED_H_
#define _SANDIX_SCHED_H_

#include <asm/processor.h>
#include <sandix/mm.h>

struct thread_info {
	struct task_struct *task;	/* main task structure */
	unsigned int flags;			/* low level flags */
	unsigned int status;		/* thread synchronous flags */
	unsigned int cpu;			/* current cpu */
};

#define TASK_RUNNING			0
#define TASK_INTERRUPTIBLE		1
#define TASK_UNINTERRRUPTIBLE	2
#define TASK_STOPPED			3
#define TASK_TRACED				4
/* exit status */
#define EXIT_ZOMBIE				5
#define EXIT_DEAD				6

/* 8 KB Stack */
#define THREAD_SIZE				(PAGE_SIZE<<1)
#define CURRENT_MASK			0xFFFFE000

struct task_struct {
	volatile int state;						/* -1 unrunnable, 0 runnable, >0 stopped */
	
	void *stack;							/* kernel-mode stack */
	
	int priority;							/* schedule priority */

	struct list_head run_list;
	
	struct list_head tasks;					/* all processes list */
	
	/*
	 * The tgid is the pid of the first thread in threadgroup.
	 * Thus the first thread in group has the pid equals to tgid.
	 */
	pid_t	pid;							/* process or thread id */
	pid_t	tgid;							/* thread group id*/

	struct task_struct	*group_leader;		/* threadgroup leader */
	struct list_head	thread_group;		/* threads in the same group */

	struct task_struct *real_parent;		/* real original parent process */
	struct task_struct *parent;				/* recipient of SIGCHLD, wait4() reports */
	struct list_head children;				/* list of my children */
	struct list_head sibling;				/* list of my parent's children */
	
	struct thread_struct thread;			/* cpu specific state of this task*/
	
	struct mm_struct *mm;					/* memory management struct */
};

union thread_union {
	struct thread_info thread_info;
	unsigned int stack[THREAD_SIZE/sizeof(int)];
};

#define task_thread_info(task)	((struct thread_info *)(task)->stack)
#define task_stack_page(task)	((task)->stack)

static inline struct thread_info *
current_thread_info()
{
	struct thread_info *__tip;
	asm volatile (
		"movl %1, %ecx\n\t"
		"andl %esp, %ecx\n\t"
		"movl %ecx, %0"
		:"=r"(__tip)
		:"i"(CURRENT_MASK)
		:"%ecx"
	);
	return __tip;
}

/*
 * current is not a variable
 * current is specific to each cpu
 */
#define current							\
({										\
	struct task_struct *__tsp;			\
	asm volatile (						\
		"movl %1, %%ecx\n\t"			\
		"andl %%esp, %%ecx\n\t"			\
		"movl (%%ecx), %0"				\
		:"=r"(__tsp)					\
		:"i"(CURRENT_MASK)				\
		:"%ecx"							\
	);									\
	__tsp;								\
})				

#define next_task(p)	\
	list_entry((p)->tasks.next, struct task, tasks)

#define for_each_process(p)	\
	for (p = &init_task; (p = next_task(p)) != &init_task; )

#endif /* _SANDIX_SCHED_H_ */
