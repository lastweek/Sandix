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

#ifndef _SANDIX_BUG_H_
#define _SANDIX_BUG_H_

#include <sandix/types.h>
#include <sandix/kernel.h>
#include <sandix/compiler.h>

#define __BUILD_BUG_ON_MSG(condition, msg, prefix, suffix)		\
do {									\
	extern void prefix##suffix(void) __compiletime_error(msg);	\
									\
	bool __cond = !!(condition);					\
	if (__cond)							\
		prefix##suffix();					\
} while (0)

#define _BUILD_BUG_ON_MSG(condition, msg, prefix, suffix)		\
	__BUILD_BUG_ON_MSG(condition, msg, prefix, suffix)

#define BUILD_BUG_ON_MSG(condition, msg)				\
	_BUILD_BUG_ON_MSG(condition, msg, __func__, __LINE__)

#define BUILD_BUG_ON(condition)						\
	BUILD_BUG_ON_MSG(condition, "BUILD_BUG_ON failed: " #condition)

/*
 * Don't use BUG() or BUG_ON() unless there's really no way out; one
 * example might be detecting data structure corruption in the middle
 * of an operation that can't be backed out of.  If the (sub)system
 * can somehow continue operating, perhaps with reduced functionality,
 * it's probably not BUG-worthy.
 *
 * If you're tempted to BUG(), think again: is completely giving up
 * really the *only* solution? There are usually better options, where
 * users don't need to reboot ASAP and can mostly shut down cleanly.
 */

#define BUG()					\
do {						\
	printk("BUG: failure at %s:%d/%s()!\n",	\
		__FILE__, __LINE__, __func__);	\
	panic("BUG!");				\
} while(0)

#define BUG_ON(condition)			\
do {						\
	if (unlikely(condition))		\
		BUG();				\
} while (0)


/*
 * WARN(), WARN_ONCE(), WARN_ON(), WARN_ON_ONCE, are used to report
 * significant issues that need prompt attention if they should ever
 * appear at runtime.
 */

#define __WARN()						\
do {								\
	printk("WARNING: at %s:%d/%s()!\n",			\
		__FILE__, __LINE__, __func__);			\
} while(0)

#define __WARN_printk(format...)				\
do {								\
	__WARN();						\
	printk(format);						\
} while (0)

#define WARN(condition, format...) ({				\
	int __ret_warn_on = !!(condition);			\
	if (unlikely(__ret_warn_on))				\
		__WARN_printk(format);				\
	unlikely(__ret_warn_on);				\
})

#define WARN_ONCE(condition, format...) ({			\
	static bool __section(.data..unlikely) __warned;	\
	int __ret_warn_once = !!(condition);			\
								\
	if (unlikely(__ret_warn_once))				\
		if (WARN(!__warned, format))			\
			__warned = true;			\
	unlikely(__ret_warn_once);				\
})

#define WARN_ON(condition) ({					\
	int __ret_warn_on = !!(condition);			\
	if (unlikely(__ret_warn_on))				\
		__WARN();					\
	unlikely(__ret_warn_on);				\
})

#define WARN_ON_ONCE(condition)	({				\
	static bool __section(.data..unlikely) __warned;	\
	int __ret_warn_once = !!(condition);			\
								\
	if (unlikely(__ret_warn_once))				\
		if (WARN_ON(!__warned)) 			\
			__warned = true;			\
	unlikely(__ret_warn_once);				\
})

#endif /* _SANDIX_BUG_H_ */
