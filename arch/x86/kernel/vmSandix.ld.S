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

#include <sandix/compiler.h>
#include <asm/page.h>

#define LOAD_OFFSET		__PAGE_OFFSET
#define	LOAD_PHYS_OFFSET	0x100000

#undef i386
OUTPUT_ARCH(i386)
ENTRY(startup_32)

SECTIONS
{
	. = LOAD_OFFSET + LOAD_PHYS_OFFSET;
	phys_startup_32 = startup_32 - LOAD_OFFSET;
	
	__kstart = .;

	.text : {
		__stext = .;
		*(.text*)
		__etext = .;
	}
	
	. = ALIGN(PAGE_SIZE);
	.rodata : {
		__srodata = .;
		*(.rodata*)
		__erodata = .;
	}

	. = ALIGN(PAGE_SIZE);
	.data : {
		__sdata = .;
		*(.data)
		
		. = ALIGN(L1_CACHE_LINE_SIZE);
		*(.data..read_mostly)
		
		__edata = .;
	}
	
	. = ALIGN(PAGE_SIZE);
	.init : {
		__sinit = .;
		*(.init.text)
		*(.init.data)
		__einit = .;
	}

	. = ALIGN(4);
	.signature : {
		kernel_sig = .;
		LONG(0x5a5aaa55)
	}

	. = ALIGN(PAGE_SIZE);
	.bss : {
		__bss_start = .;
		*(.bss)
		__bss_end = .;
	}

	. = ALIGN(PAGE_SIZE);
	.brk : {
		__brk_start = .;
		. += 64 * 1024;
		__brk_end = .;
	}

	__kend = .;
	
	/DISCARD/ : { *(.note*) }
	/DISCARD/ : { *(.eh_frame) }
	/DISCARD/ : { *(.comment) }
}
