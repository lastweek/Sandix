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

#ifndef _ASM_GENERIC_SWITCH_TO_H_
#define _ASM_GENERIC_SWITCH_TO_H_

/*
 * Generic task switch macro wrapper for simple architecture
 */

extern struct task_struct * __switch_to(struct task_struct *,
					struct task_struct *);

#define switch_to(prev, next, last)			\
	do {						\
		(last) = __switch_to((prev), (next));	\
	} while (0)

#endif /* _ASM_GENERIC_SWITCH_TO_H_ */
