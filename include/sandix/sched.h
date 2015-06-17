#ifndef _SANDIX_SCHED_H_
#define _SANDIX_SCHED_H_

struct task_struct {
	volatile long state;	/* -1 unrunnable, 0 runnable, >0 stopped */
}

#endif /* _SANDIX_SCHED_H_ */
