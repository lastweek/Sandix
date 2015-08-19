/*
 *	include/sandix/kernel.h - Useful Kernel Definitions
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

#ifndef _SANDIX_KERNEL_H_
#define _SANDIX_KERNEL_H_

#include <sandix/compiler.h>
#include <sandix/linkage.h>
#include <sandix/types.h>

#define offsetof(TYPE, MEMBER) \
	(((unsigned int)) &((TYPE *)0)->MEMBER)

/**
 * container_of - find which structure this @ptr located
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member)						\
	({														\
		typeof( ((type *)0)->member ) *__ptr = (ptr);		\
		(type *)( (char *)__ptr - offsetof(type,member) );	\
	})

#endif /* _SANDIX_KERNEL_H_ */
