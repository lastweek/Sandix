/*
 *	Variable Arguments
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

#ifndef _STDARG_H_
#define _STDARG_H_

#define __STACK_ALIGN		sizeof(long)
#define __va_rounded_size(TYPE) \
	(((sizeof(TYPE) + __STACK_ALIGN - 1) / __STACK_ALIGN) * __STACK_ALIGN)

/*
 * Variable Argument Manipulation API
 */
typedef char *va_list;
#define va_arg(AP, TYPE)				\
	({						\
		TYPE __ret;				\
		__ret = *((TYPE *)(AP));		\
		(AP) += __va_rounded_size(TYPE);	\
		__ret;					\
	})
#define va_start(AP, LAST)	(AP) = ((va_list)&(LAST) + __va_rounded_size(LAST))
#define va_end(AP)		(AP) = NULL
#define va_copy(DEST, SRC)	(DEST) = (SRC)

#endif /* _STDARG_H_ */
