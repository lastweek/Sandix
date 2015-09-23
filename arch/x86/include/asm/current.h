/*
 *	Copyright (C) 2015 Yizhou Shan <shanyizhou@ict.ac.cn>
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

#ifndef _ASM_X86_CURRENT_H_
#define _ASM_X86_CURRENT_H_

#ifndef _SANDIX_SCHED_H_
# error INCLUDE ERROR: Do NOT Include This File Directly.
#endif

static inline struct thread_info * current_thread_info()
{
	int d0;
	struct thread_info *__tip;
	asm volatile (
		"movl %2, %%ecx\n\t"
		"andl %%esp, %%ecx\n\t"
		"movl %%ecx, %0"
		: "=r" (__tip), "=c" (d0)
		: "i" (CURRENT_MASK)
	);
	return __tip;
}

#define current					\
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

#define current	get_current()

#endif /* _ASM_X86_CURRENT_H_ */
