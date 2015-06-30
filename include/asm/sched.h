#ifndef _ASM_SCHED_H_
#define _ASM_SCHED_H_

struct thread_info;
struct task_struct;

static inline struct thread_info *
native_current_thread_info()
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
#define native_current					\
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

#endif /* _ASM_SCHED_H_ */
