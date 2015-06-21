#ifndef _SANDIX_SCHED_H_
#define _SANDIX_SCHED_H_

#include <asm/processor.h>

struct task_struct {
	volatile long state;	/* -1 unrunnable, 0 runnable, >0 stopped */

	/* cpu specific state of this task */
	struct thread_struct thread;
}

#endif /* _SANDIX_SCHED_H_ */
