/*
 *	Copyright (C) 2015-2016 Yizhou Shan <shan13@purdue.edu>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 */

#ifndef _SANDIX_SCHED_H_
#define _SANDIX_SCHED_H_

#include <sandix/mm.h>
#include <sandix/list.h>
#include <sandix/page.h>
#include <sandix/magic.h>
#include <sandix/types.h>
#include <sandix/atomic.h>
#include <sandix/threads.h>
#include <sandix/cpumask.h>
#include <sandix/spinlock.h>
#include <sandix/compiler.h>
#include <sandix/thread_info.h>

#include <asm/current.h>
#include <asm/processor.h>

/*
 * Scheduling policies
 */
#define SCHED_NORMAL		0
#define SCHED_FIFO		1
#define SCHED_RR		2
#define SCHED_BATCH		3
#define SCHED_IDLE		4
#define SCHED_DEADLINE		5

/*
 * We have two separate sets of flags: task->state
 * is about runnability, while task->exit_state are
 * about the task exiting. Confusing, but this way
 * modifying one set can't modify the other one by
 * mistake.
 */
#define TASK_RUNNING		0
#define TASK_INTERRUPTIBLE	1
#define TASK_UNINTERRUPTIBLE	2
#define __TASK_STOPPED		4
#define __TASK_TRACED		8
/* in task->exit_state */
#define EXIT_DEAD		16
#define EXIT_ZOMBIE		32
#define EXIT_TRACE		(EXIT_DEAD | EXIT_ZOMBIE)
/* in task->state again */
#define TASK_DEAD		64
#define TASK_WAKEKILL		128
#define TASK_WAKING		256
#define TASK_PARKED		512
#define TASK_NOLOAD		1024
#define TASK_STATE_MAX		2048

/* Convenience macros for set_task_state */
#define TASK_KILLABLE		(TASK_WAKEKILL | TASK_UNINTERRUPTIBLE)
#define TASK_STOPPED		(TASK_WAKEKILL | __TASK_STOPPED)
#define TASK_TRACED		(TASK_WAKEKILL | __TASK_TRACED)
#define TASK_IDLE		(TASK_UNINTERRUPTIBLE | TASK_NOLOAD)

/* Convenience macros for wake_up */
#define TASK_NORMAL		(TASK_INTERRUPTIBLE | TASK_UNINTERRUPTIBLE)
#define TASK_ALL		(TASK_NORMAL | __TASK_STOPPED | __TASK_TRACED)

/* Convenience macros for get_task_state */
#define TASK_REPORT		(TASK_RUNNING | TASK_INTERRUPTIBLE | \
				 TASK_UNINTERRUPTIBLE | __TASK_STOPPED | \
				 __TASK_TRACED | EXIT_ZOMBIE | EXIT_DEAD)

#define task_is_traced(task)	((task->state & __TASK_TRACED) != 0)
#define task_is_stopped(task)	((task->state & __TASK_STOPPED) != 0)

/*
 * set_current_state() includes a barrier so that the write of current->state
 * is correctly serialised wrt the caller's subsequent test of whether to
 * actually sleep:
 *
 *	set_current_state(TASK_UNINTERRUPTIBLE);
 *	if (do_i_need_to_sleep())
 *		schedule();
 *
 * If the caller does not need such serialisation then use __set_current_state()
 */
#define set_task_state(task, state_value)
#define set_current_state(state_value)

#define __set_task_state(task, state_value)	\
	do {					\
		(task)->state = (state_value);	\
	} while (0)

#define __set_current_state(state_value)	\
	do {					\
		current->state = (state_value);	\
	} while (0)

/* Track pages that require TLB flushes */
struct tlbflush_unmap_batch {
	/*
	 * Each bit set is a CPU that potentially has a TLB entry for one of
	 * the PFNs being flushed. See set_tlb_ubc_flush_pending().
	 */
	struct cpumask cpumask;

	/* True if any bit in cpumask is set */
	bool flush_required;

	/*
	 * If true then the PTE was dirty when unmapped. The entry must be
	 * flushed before IO is initiated or a stale TLB entry potentially
	 * allows an update without redirtying the page.
	 */
	bool writable;
};

#define TASK_COMM_LEN	32

struct task_struct {
	/* -1 unrunnable, 0 runnable, >0 stopped */
	volatile long state;

	char comm[TASK_COMM_LEN];

	/* kernel mode stack */
	void *stack;

	atomic_t usage;
	unsigned int ptrace;

	/* per-process flag */
	unsigned int flags;

#ifdef CONFIG_SMP
	int on_cpu;
	int wake_cpu;
#endif

	int prio, static_prio, normal_prio;
	unsigned int policy;
	int nr_cpus_allowed;
	cpumask_t cpus_allowed;

	struct list_head tasks;

	struct mm_struct *mm, *active_mm;
	
	/* protect mm, fs, etc. */
	spinlock_t alloc_lock;

	/* task state */
	int exit_state;
	int exit_codem, exit_signal;

	/*
	 * The tgid is the pid of the first thread in thread group.
	 * Thus the first thread in group has the pid equals to tgid.
	 */
	pid_t	pid;
	pid_t	tgid;

	/* real parent process */
	struct task_struct *real_parent;

	/* recipient of SIGCHLD */
	struct task_struct *parent;

	/* list of my children */
	struct list_head children;

	/* linkage in my parent's children list */
	struct list_head sibling;

	/* thread group leader */
	struct task_struct *group_leader;

	/* CPU-specific state of this task */
	struct thread_struct thread;
};

/*
 * Protects ->fs, ->files, ->mm, ->group_info, ->comm, keyring
 * subscriptions and synchronises with wait4().  Also used in procfs.  Also
 * pins the final release of task.io_context.  Also protects ->cpuset and
 * ->cgroup.subsys[]. And ->vfork_done.
 *
 * Nests both inside and outside of read_lock(&tasklist_lock).
 * It must not be nested with write_lock_irq(&tasklist_lock),
 * neither inside nor outside.
 */
static inline void task_lock(struct task_struct *task)
{
	spin_lock(&task->alloc_lock);
}

static inline void task_unlock(struct task_struct *task)
{
	spin_unlock(&task->alloc_lock);
}

static inline pid_t task_pid_t(struct task_struct *task)
{
	return task->pid;
}

static inline pid_t task_tgid_t(struct task_struct *task)
{
	return task->tgid;
}

/**
 * is_global_init - check if a task structure is init. Since init
 * is free to have sub-threads we need to check tgid.
 * @tsk: Task structure to be checked.
 *
 * Check if a task structure is the first user space task the kernel created.
 *
 * Return: 1 if the task structure is init. 0 otherwise.
 */
static inline int is_global_init(struct task_struct *task)
{
	return task_tgid_t(task) == 1;
}

static inline void get_task_struct(struct task_struct *task)
{
	atomic_inc(&task->usage);
}

static inline void put_task_struct(struct task_struct *task)
{	/* XXX <=0 free or bug? */
	atomic_dec(&task->usage);
}

/*
 * Per process flags
 */
#define PF_EXITING		0x00000004	/* getting shut down */
#define PF_EXITPIDONE		0x00000008	/* pi exit done on shut down */
#define PF_VCPU			0x00000010	/* I'm a virtual CPU */
#define PF_WQ_WORKER		0x00000020	/* I'm a workqueue worker */
#define PF_FORKNOEXEC		0x00000040	/* forked but didn't exec */
#define PF_MCE_PROCESS  	0x00000080      /* process policy on mce errors */
#define PF_SUPERPRIV		0x00000100	/* used super-user privileges */
#define PF_DUMPCORE		0x00000200	/* dumped core */
#define PF_SIGNALED		0x00000400	/* killed by a signal */
#define PF_MEMALLOC		0x00000800	/* Allocating memory */
#define PF_NPROC_EXCEEDED	0x00001000	/* set_user noticed that RLIMIT_NPROC was exceeded */
#define PF_USED_MATH		0x00002000	/* if unset the fpu must be initialized before use */
#define PF_USED_ASYNC		0x00004000	/* used async_schedule*(), used by module init */
#define PF_NOFREEZE		0x00008000	/* this thread should not be frozen */
#define PF_FROZEN		0x00010000	/* frozen for system suspend */
#define PF_FSTRANS		0x00020000	/* inside a filesystem transaction */
#define PF_KSWAPD		0x00040000	/* I am kswapd */
#define PF_MEMALLOC_NOIO	0x00080000	/* Allocating memory without IO involved */
#define PF_LESS_THROTTLE	0x00100000	/* Throttle me less: I clean memory */
#define PF_KTHREAD		0x00200000	/* I am a kernel thread */
#define PF_RANDOMIZE		0x00400000	/* randomize virtual address space */
#define PF_SWAPWRITE		0x00800000	/* Allowed to write to swap */
#define PF_NO_SETAFFINITY	0x04000000	/* Userland is not allowed to meddle with cpus_allowed */
#define PF_MCE_EARLY		0x08000000      /* Early kill for mce process policy */
#define PF_MUTEX_TESTER		0x20000000	/* Thread belongs to the rt mutex tester */
#define PF_FREEZER_SKIP		0x40000000	/* Freezer should not count it as freezable */
#define PF_SUSPEND_TASK		0x80000000      /* this thread called freeze_processes and should not be frozen */

union thread_union {
	struct thread_info thread_info;
	unsigned long stack[THREAD_SIZE/sizeof(long)];
};

extern union thread_union init_thread_union;
extern struct task_struct init_task;
extern struct mm_struct   init_mm;

#define next_task(p)	\
	list_entry((p)->tasks.next, struct task_struct, tasks)

#define for_each_process(p)	\
	for (p = &init_task; (p = next_task(p)) != &init_task; )

#define task_stack_page(task)	((task)->stack)
#define task_thread_info(task)	((struct thread_info *)(task)->stack)

static inline void setup_thread_stack(struct task_struct *p, struct task_struct *orig)
{
	*task_thread_info(p) = *task_thread_info(orig);
	task_thread_info(p)->task = p;
}

/*
 * Returns the address of the last usable long on the stack.
 * The stack grows down, this is just above the thread_info
 * struct. Going any lower will corrupt the thread_info.
 */
static inline unsigned long *end_of_stack(struct task_struct *task)
{
	return (unsigned long *)(task_thread_info(task) + 1);
}

static inline int task_stack_end_corrupted(struct task_struct *task)
{
	return *(end_of_stack(task)) != STACK_END_MAGIC;
}

static inline int object_is_on_stack(void *obj)
{
	void *stack = task_stack_page(current);

	return (obj >= stack) && (obj < (stack + THREAD_SIZE));
}

/*
 * set/clear/test thread flags in other task's structures
 * see asm/thread_info.h for TIF_xxxx flags
 */
static inline void set_task_thread_flag(struct task_struct *task, int flag)
{
	set_ti_thread_flag(task_thread_info(task), flag);
}

static inline void clear_task_thread_flag(struct task_struct *task, int flag)
{
	clear_ti_thread_flag(task_thread_info(task), flag);
}

static inline int test_task_thread_flag(struct task_struct *task, int flag)
{
	return test_ti_thread_flag(task_thread_info(task), flag);
}

#ifndef TASK_SIZE_OF
#define TASK_SIZE_OF(task)	TASK_SIZE
#endif

/**
 * Priority
 */

#define MAX_NICE	19
#define MIN_NICE	-20
#define NICE_WIDTH	(MAX_NICE - MIN_NICE + 1)

/*
 * Priority of a process goes from 0..MAX_PRIO-1, valid RT
 * priority is 0..MAX_RT_PRIO-1, and SCHED_NORMAL/SCHED_BATCH
 * tasks are in the range MAX_RT_PRIO..MAX_PRIO-1. Priority
 * values are inverted: lower p->prio value means higher priority.
 *
 * The MAX_USER_RT_PRIO value allows the actual maximum
 * RT priority to be separate from the value exported to
 * user-space.  This allows kernel threads to set their
 * priority to a value higher than any user task. Note:
 * MAX_RT_PRIO must not be smaller than MAX_USER_RT_PRIO.
 */

#define MAX_USER_RT_PRIO	100
#define MAX_RT_PRIO		MAX_USER_RT_PRIO

#define MAX_PRIO		(MAX_RT_PRIO + NICE_WIDTH)
#define DEFAULT_PRIO		(MAX_RT_PRIO + NICE_WIDTH / 2)

/*
 * Convert user-nice values [ -20 ... 0 ... 19 ]
 * to static priority [ MAX_RT_PRIO..MAX_PRIO-1 ],
 * and back.
 */
#define NICE_TO_PRIO(nice)	((nice) + DEFAULT_PRIO)
#define PRIO_TO_NICE(prio)	((prio) - DEFAULT_PRIO)

/*
 * 'User priority' is the nice value converted to something we
 * can work with better when scaling various scheduler parameters,
 * it's a [ 0 ... 39 ] range.
 */
#define USER_PRIO(p)		((p)-MAX_RT_PRIO)
#define TASK_USER_PRIO(p)	USER_PRIO((p)->static_prio)
#define MAX_USER_PRIO		(USER_PRIO(MAX_PRIO))

/*
 * Convert nice value [19,-20] to rlimit style value [1,40].
 */
static inline long nice_to_rlimit(long nice)
{
	return (MAX_NICE - nice + 1);
}

/*
 * Convert rlimit style value [1,40] to nice value [-20, 19].
 */
static inline long rlimit_to_nice(long prio)
{
	return (MAX_NICE - prio + 1);
}

void set_task_stack_end_magic(struct task_struct *task);

void __init trap_init(void);

#endif /* _SANDIX_SCHED_H_ */
