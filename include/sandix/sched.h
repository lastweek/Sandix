#ifndef _SANDIX_SCHED_H_
#define _SANDIX_SCHED_H_

#include <asm/processor.h>
#include <sandix/thread_info.h>

#define TASK_RUNNING			0
#define TASK_INTERRUPTIBLE		1
#define TASK_UNINTERRRUPTIBLE	2

struct task_struct {
	volatile long state;	/* -1 unrunnable, 0 runnable, >0 stopped */
	void *stack;

	pid_t	pid;
	pid_t	tgid;

	/* cpu specific state of this task */
	struct thread_struct thread;
}

#endif /* _SANDIX_SCHED_H_ */
