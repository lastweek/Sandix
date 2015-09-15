/*
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

#ifndef _ASM_X86_BOOT_H_
#define _ASM_X86_BOOT_H_

#include <sandix/const.h>
#include <asm/page.h>

/* Physical address where kernel is loaded */
#define LOAD_PHYSICAL_ADDR	0x100000

/* Minimum kernel alignment, as a power of two */
#define MIN_KERNEL_ALIGN_LG2	(PAGE_SHIFT + THREAD_PAGE_NR)
#define MIN_KERNEL_ALIGN	(_AC(1, UL) << MIN_KERNEL_ALIGN_LG2)

#define BOOT_STACK_SIZE		0x1000

#define COMMAND_LINE_SIZE	2048

#endif /* _ASM_X86_BOOT_H_ */
