/*
 *	Copyright (C) 2015-2016 Yizhou Shan <shan13@purdue.edu>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 */

#include <sandix/magic.h>
#include <sandix/sched.h>
#include <sandix/compiler.h>

#include <asm/switch_to.h>

void set_task_stack_end_magic(struct task_struct *task)
{
	unsigned long *ptr = end_of_stack(task);
	*ptr = STACK_END_MAGIC;
}
