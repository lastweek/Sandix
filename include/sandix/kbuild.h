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

#ifndef _SANDIX_KBUILD_H_
#define _SANDIX_KBUILD_H_

/*
 * Used to generate assembly offset macros
 */

#define BLANK()				\
	asm volatile (			\
		"\n->"			\
		: :			\
	)

#define COMMENT(x)			\
	asm volatile (			\
		"\n->#" x		\
	)

#define DEFINE(sym, val)		\
	asm volatile (			\
		"\n->" #sym " %0 " #val	\
		: : "i" (val)		\
	)

#define OFFSET(sym, str, mem)		\
	DEFINE(sym, offsetof(struct str, mem))

#endif /* _SANDIX_KBUILD_H_ */
