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

#ifndef _SANDIX_PAGE_H_
#define _SANDIX_PAGE_H_

#include <asm/page.h>

#include <sandix/types.h>
#include <sandix/atomic.h>

enum SANDIX_PAGE_FLAGS {
	PG_present,
	PG_xxxx
};

/*
 * Each physical page in the system has a struct page
 * associated with it to keep track of whatever it is
 * we are using the page for at the moment.
 */
struct page {
	unsigned long flag;
	atomic_t count;
	void *virtual;
};

#endif /* _SANDIX_PAGE_H_*/
