/*
 *	Copyright (C) 2015-2016 Yizhou Shan <shan13@purdue.edu>
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

#ifndef _SANDIX_KREF_H_
#define _SANDIX_KREF_H_

#include <sandix/atomic.h>
#include <sandix/compiler.h>
#include <sandix/types.h>

/* TODO used by tty part now */

struct kref {
	atomic_t refcount;
};

static __always_inline void kref_init(struct kref *ref)
{
	atomic_set(&ref->refcount, 1);
}

static __always_inline void kref_get(struct kref *ref)
{

}

static __always_inline void kref_put(struct kref *ref)
{

}

#endif /* _SANDIX_KREF_H_ */
