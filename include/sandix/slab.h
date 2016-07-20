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

#ifndef _SANDIX_SLAB_H_
#define _SANDIX_SLAB_H_

#include <sandix/types.h>
#include <sandix/compiler.h>

static __always_inline void *kmalloc(size_t size, int flags)
{
	return NULL;
}

static __always_inline void kfree(void *ptr)
{

}

/*
 * State of the slab allocator.
 *
 * This is used to describe the states of the allocator during bootup.
 * Allocators use this to gradually bootstrap themselves. Most allocators
 * have the problem that the structures used for managing slab caches are
 * allocated from slab caches themselves.
 */
enum SLAB_STATES {
	SLAB_DOWN,			/* No slab functionality yet */
	SLAB_PARTIAL,			/* SLUB: kmem_cache_node available */
	SLAB_PARTIAL_NODE,		/* SLAB: kmalloc size for node struct available */
	SLAB_UP,			/* Slab caches usable but not all extras yet */
	SLAB_FULL			/* Everything is working */
};

bool slab_is_available(void);

#endif /* _SANDIX_SLAB_H_ */
