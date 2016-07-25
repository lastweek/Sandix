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

/*
 * Things about page->flags
 */

#ifndef _SANDIX_PAGE_FLAGS_H_
#define _SANDIX_PAGE_FLAGS_H_

#include <sandix/bitops.h>

enum PAGE_FLAGS {
	PG_locked,
	PG_referenced,
	PG_uptodate,
	PG_dirty,
	PG_lru,
	PG_active,
	PG_slab,
	PG_writeback,
	PG_head,
	PG_unevictable,

	__NR_PAGE_FLAGS
};

struct page;

#define TEST_PAGE_FLAG(uname, lname)					\
static __always_inline int Page##uname(struct page *page)		\
{									\
	return test_bit(PG_##lname, &page->flags);			\
}

#define SET_PAGE_FLAG(uname, lname)					\
static __always_inline void SetPage##uname(struct page *page)		\
{									\
	set_bit(PG_##lname, &page->flags);				\
}

#define CLEAR_PAGE_FLAG(uname, lname)					\
static __always_inline void ClearPage##uname(struct page *page)		\
{									\
	clear_bit(PG_##lname, &page->flags);				\
}

#define __SET_PAGE_FLAG(uname, lname)					\
static __always_inline void __SetPage##uname(struct page *page)		\
{									\
	__set_bit(PG_##lname, &page->flags);				\
}

#define __CLEAR_PAGE_FLAG(uname, lname)					\
static __always_inline void __ClearPage##uname(struct page *page)	\
{									\
	__clear_bit(PG_##lname, &page->flags);				\
}

#define TEST_AND_SET_PAGE_FLAG(uname, lname)				\
static __always_inline int TestSetPage##uname(struct page *page)	\
{									\
	return test_and_set_bit(PG_##lname, &page->flags);		\
}

#define TEST_AND_CLEAR_PAGE_FLAG(uname, lname)				\
static __always_inline int TestClearPage##uname(struct page *page)	\
{									\
	return test_and_clear_bit(PG_##lname, &page->flags);		\
}

#define SANDIX_PAGE_FLAG(uname, lname)				\
	TEST_PAGE_FLAG(uname, lname)				\
	SET_PAGE_FLAG(uname, lname)				\
	CLEAR_PAGE_FLAG(uname, lname)				\
	__SET_PAGE_FLAG(uname, lname)				\
	__CLEAR_PAGE_FLAG(uname, lname)				\
	TEST_AND_SET_PAGE_FLAG(uname, lname)			\
	TEST_AND_CLEAR_PAGE_FLAG(uname, lname)

SANDIX_PAGE_FLAG(Locked, locked)
SANDIX_PAGE_FLAG(Referenced, referenced)
SANDIX_PAGE_FLAG(Uptodate, uptodate)
SANDIX_PAGE_FLAG(Dirty, dirty)
SANDIX_PAGE_FLAG(LRU, lru)
SANDIX_PAGE_FLAG(Active, active)
SANDIX_PAGE_FLAG(Slab, slab)
SANDIX_PAGE_FLAG(Writeback, writeback)
SANDIX_PAGE_FLAG(Head, head)
SANDIX_PAGE_FLAG(Unevictable, unevictable)

#endif /* _SANDIX_PAGE_FLAGS_H_ */
