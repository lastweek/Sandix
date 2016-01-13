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

#ifndef _ASM_X86_ASM_H_
#define _ASM_X86_ASM_H_

#include <sandix/compiler.h>

#define LOCK_PREFIX "lock; "

#define nop()	native_nop()
#define mb()	native_mb()
#define rmb()	native_rmb()
#define wmb()	native_wmb()

static __always_inline void native_nop(void)
{
	asm volatile ("nop");
}

static __always_inline void native_mb(void)
{
	asm volatile ("mfence");
}

static __always_inline void native_rmb(void)
{
	asm volatile ("lfence");
}

static __always_inline void native_wmb(void)
{
	asm volatile ("wfence");
}

#endif /* _ASM_X86_ASM_H_ */
