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

#ifndef _SANDIX_MM_H_
#define _SANDIX_MM_H_

//#include <asm/pgtable.h>

#include <sandix/page.h>
#include <sandix/types.h>
#include <sandix/atomic.h>
#include <sandix/spinlock.h>

struct mm_struct;

/*
 * This struct defines a memory VMM memory area. There is one of these
 * per VM-area/task.  A VM area is any part of the process virtual memory
 * space that has a special rule for the page-fault handlers (ie a shared
 * library, the executable area etc).
 */
struct vm_area_struct {
	unsigned long vm_start, vm_end;
	struct vm_area_struct *vm_next, *vm_prev;
	struct mm_struct *vm_mm;
};

/**
 * struct mm_struct
 * @mmap:		list of VMAs
 * @mmap_base:		base of mmap area
 * @pgd:		global page table
 * @mm_users:		how many users with userspace
 * @mm_count:		how many references to this struct
 * @page_table_lock:	protects page table and some counters
 */
struct mm_struct {
	struct vm_area_struct	*mmap;
	//pgd_t 			*pgd;
	atomic_t		mm_users;
	atomic_t		mm_count;
	spinlock_t		page_table_lock;
	unsigned long		mmap_base;
	unsigned long		start_code, end_code;
	unsigned long		start_data, end_data;
	unsigned long		start_brk, brk;
	unsigned long		start_stack;
};

extern unsigned long max_low_pfn;
extern unsigned long min_low_pfn;
extern unsigned long max_pfn;

#endif /* _SANDIX_MM_H_ */
