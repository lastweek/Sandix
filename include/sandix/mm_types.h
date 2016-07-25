/*
 *	Copyright (C) 2016 Yizhou Shan <shan13@purdue.edu>
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

#ifndef _SANDOX_MM_TYPES_H_
#define _SANDOX_MM_TYPES_H_

/*
 * Each physical page in the system has a struct page associated with
 * it to keep track of whatever it is we are using the page for at the
 * moment. Note that we have no way to track which tasks are using
 * a page, though if it is a pagecache page, rmap structures can tell us
 * who is mapping it.
 *
 * The objects in struct page are organized in double word blocks in
 * order to allows us to use atomic double word operations on portions
 * of struct page. That is currently only used by slub but the arrangement
 * allows the use of atomic double word operations on the flags/mapping
 * and lru list pointers also.
 */
struct page {
	unsigned long flags;
};

/*
 * This struct defines a memory VMM memory area. There is one of these
 * per VM-area/task.  A VM area is any part of the process virtual memory
 * space that has a special rule for the page-fault handlers (ie a shared
 * library, the executable area etc).
 */
struct vm_area_struct {
	unsigned long		vm_start;
	unsigned long		vm_end;
	struct vm_area_struct	*vm_next;
	struct vm_area_struct	*vm_prev;
	unsigned long		vm_flags;
};

struct mm_struct {
	struct vm_area_struct	*mmap;
	pgd_t 			*pgd;
	atomic_t		mm_users;
	atomic_t		mm_count;
	spinlock_t		page_table_lock;
	unsigned long		mmap_base;
	unsigned long		start_code, end_code;
	unsigned long		start_data, end_data;
	unsigned long		start_brk, brk;
	unsigned long		start_stack;
};

#endif /* _SANDOX_MM_TYPES_H_ */
