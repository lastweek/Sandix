/*
 *	Copyright (C) 2015-2016 Yizhou Shan <shan13@purdue.edu>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 */

#ifndef _ASM_X86_CURRENT_H_
#define _ASM_X86_CURRENT_H_

#include <sandix/compiler.h>
#include <sandix/thread_info.h>

struct task_struct;

static __always_inline struct task_struct *get_current(void)
{
	return current_thread_info()->task;
}

#define current	get_current()

#endif /* _ASM_X86_CURRENT_H_ */
