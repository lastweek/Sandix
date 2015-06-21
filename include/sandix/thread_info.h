#ifndef _SANDIX_THREAD_INFO_H_
#define _SANDIX_THREAD_INFO_H_

struct thread_info {
	struct task_struct *task;	/* main task structure */
	__u32		flags;			/* low level flags */
	__u32		status;			/* thread synchronous flags */
	__u32		cpu;			/* current cpu */
};

#endif /* _SANDIX_THREAD_INFO_H_ */
