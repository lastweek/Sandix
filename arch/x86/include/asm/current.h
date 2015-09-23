#ifndef _ASM_SCHED_H_
#define _ASM_SCHED_H_

struct thread_info;
struct task_struct;

static inline struct thread_info *
native_current_thread_info()
{
	int d0;
	struct thread_info *__tip;
	asm volatile (
		"movl %2, %ecx\n\t"
		"andl %esp, %ecx\n\t"
		"movl %ecx, %0"
		: "=r" (__tip), "=c" (d0)
		: "i" (CURRENT_MASK)
	);
	return __tip;
}

/*
 * current is not a variable
 * current is specific to each cpu
 */
#define native_current				\
({						\
	int d0;					\
	struct task_struct *__tsp;		\
	asm volatile (				\
		"movl %2, %%ecx\n\t"		\
		"andl %%esp, %%ecx\n\t"		\
		"movl (%%ecx), %0"		\
		: "=r" (__tsp), "=c" (d0)	\
		: "i" (CURRENT_MASK)		\
	);					\
	__tsp;					\
})				

#endif /* _ASM_SCHED_H_ */
