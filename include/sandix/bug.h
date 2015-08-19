/*
 *	include/sandix/bug.h - What a BUG!
 *
 *	Copyright (C) 2015 Yizhou Shan <shanyizhou@ict.ac.cn>
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

#ifndef _SANDIX_BUG_H_
#define _SANDIX_BUG_H_

/*
 * Don't use BUG() or BUG_ON() unless there's really no way out; one
 * example might be detecting data structure corruption in the middle
 * of an operation that can't be backed out of.  If the (sub)system
 * can somehow continue operating, perhaps with reduced functionality,
 * it's probably not BUG-worthy.
 *
 * If you're tempted to BUG(), think again:  is completely giving up
 * really the *only* solution?  There are usually better options, where
 * users don't need to reboot ASAP and can mostly shut down cleanly.
 */

#define BUG()					\
do {						\
	printk("BUG: failure at %s:%d/%s()!\n",	\
		__FILE__, __LINE__, __func__);	\
} while(0)

#define BUG_ON(condition)			\
do {						\
	if (unlikely(condition))		\
		BUG();				\
} while (0)

/*
 * WARN(), WARN_ON(), WARN_ON_ONCE, and so on can be used to report
 * significant issues that need prompt attention if they should ever
 * appear at runtime.
 */

#define WARN()

#define WARN_ON_ONCE()

#define WARN_ON()

#endif /* _SANDIX_BUG_H_ */
