/*
 *	Copyright (C) 2015-2016 Yizhou Shan <shan13@purdue.edu>
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

#include <asm/e820.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/sections.h>

#include <sandix/mm.h>
#include <sandix/kernel.h>

unsigned long __init init_memory_mapping(unsigned long start,
					 unsigned long end)
{
	printk(KERN_DEBUG "init_mmeory_mapping: [mem %#010lx-%#010lx]\n",
		start, end - 1);
}

void __init init_mem_mapping(void)
{
	unsigned long end;
	unsigned long kernel_end = __pa(__kend);

#ifdef CONFIG_X86_32
	end = max_low_pfn << PAGE_SHIFT;
#else
	end = max_pfn << PAGE_SHIFT;
#endif

	/* the ISA range is always mapped regardless of memory holes */
	init_memory_mapping(0, ISA_END_ADDRESS);


}
