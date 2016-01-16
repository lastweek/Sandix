/*
 *	Copyright (C) 2015-2016 Yizhou Shan <shanyizhou@ict.ac.cn>
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

#ifndef _SANDIX_RWSEM_H_
#define _SANDIX_RWSEM_H_

#include <sandix/types.h>
#include <sandix/compiler.h>
#include <sandix/spinlock.h>

/* TODO now used by tty part */

struct rw_semaphore {
	long			count;
	struct list_head	wait_list;
	spinlock_t		wait_lock;
};

#define DEFINE_RWSEM(name) \
	struct rw_semaphore name

#define rwsem_init(name)

static inline void down_read(struct rw_semaphore *sem)
{}
static inline void up_read(struct rw_semaphore *sem)
{}
static inline void down_write(struct rw_semaphore *sem)
{}
static inline void up_write(struct rw_semaphore *sem)
{}

#endif /* _SANDIX_RWSEM_H_ */
