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

/*
 * Low-level thread information structure helpers
 */

#ifndef _SANDIX_THREAD_INFO_
#define _SANDIX_THREAD_INFO_

#include <sandix/types.h>
#include <sandix/bitops.h>
#include <sandix/compiler.h>

#include <asm/page.h>
#include <asm/thread_info.h>

/*
 * set/clear/test flag wrappers
 * pass TIF_xxx constants to these functions
 */

static inline void set_ti_thread_flag(struct thread_info *ti, int flag)
{
	set_bit(flag, &ti->flags);
}

static inline void clear_ti_thread_flag(struct thread_info *ti, int flag)
{
	clear_bit(flag, &ti->flags);
}

static inline int test_ti_thread_flag(struct thread_info *ti, int flag)
{
	return test_bit(flag, &ti->flags);
}

static inline void set_thread_flag(int flag)
{
	set_ti_thread_flag(current_thread_info(), flag);
}

static inline void clear_thread_flag(int flag)
{
	clear_thread_flag(current_thread_info(), flag);
}

static inline int test_thread_flag(int flag)
{
	return test_ti_thread_flag(current_thread_info(), flag);
}

#endif /* _SANDIX_THREAD_INFO_ */
