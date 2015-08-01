/*
 *	Sandix Linker Script - Assembly Version
 *
 *	BIG FAT NOTE: Make sure you do not eliminate sections you need!!!
 *	If you happenly miss something, it will lead to hard-to-debug errors.
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

#include <sandix/compiler.h>
#include <asm/page.h>

/*
 * Kernel Virtual Address Offset.
 * In 32-bit system, this is 0xC0000000, which is the PAGE_OFFSET.
 * And that's why we have NORMAL_MEM and HIGH_MEM! I really confused about why
 * just do not using 0x0 as kernel virtual address basis? Then kernel do not
 * have to distinguish between normal and high memory!
 */
/* LOAD_OFFSET = PAGE_OFFSET */

#define LOAD_OFFSET		0x0
#define	LOAD_PHYS_OFFSET	0x10000

#undef i386
OUTPUT_ARCH(i386)
ENTRY(startup_32)
SECTIONS
{
	. = LOAD_OFFSET + LOAD_PHYS_OFFSET;
	phys_startup_32 = startup_32 - LOAD_OFFSET;
	
	__kstart__ = .;
	.text : {
		__stext__ = .;
		*(.text)
		__etext__ = .;
	}
	
	/* Add wildcard after rodata. GCC will add f*ing suffix */
	. = ALIGN(PAGE_SIZE);
	.rodata : {
		__srodata__ = .;
		*(.rodata*)
		__erodata__ = .;
	}

	. = ALIGN(PAGE_SIZE);
	.data : {
		__sdata__ = .;
		*(.data)
		. = ALIGN(L1_CACHE_LINE_SIZE);
		*(.data..read_mostly)
		__edata__ = .;
	}
	
	/* Init data and text sections. Can be freed later */
	. = ALIGN(PAGE_SIZE);
	.init : {
		__sinit__ = .;
		*(.init.text)
		*(.init.data)
		__einit = .;
	}

	. = ALIGN(PAGE_SIZE);
	.bss :
	{
		__bss_start__ = .;
		*(.bss)
		__bss_end__ = .;
	}

	/DISCARD/ : { *(.note*) }
	/DISCARD/ : { *(.eh_frame) }
	/DISCARD/ : { *(.comment) }
	
	__kend__ = .;
}
