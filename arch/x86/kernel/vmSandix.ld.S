#
#	Sandix Linker Script - Assembly Version
#
#	vmSandix has 5 sections:
#		.text		-	Kernel Built-in Code
#		.rodata		-	Kernel Read-Only Data
#		.data		-	Kernel Writable Data
#		.init		-	Kernel Init Code
#		.bss		-	BSS Section
#
#	Copyright (C) 2015 Yizhou Shan <shanyizhou@ict.ac.cn>
#
#	This program is free software; you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation; either version 2 of the License, or
#	(at your option) any later version.
#
#	This program is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License along
#	with this program; if not, write to the Free Software Foundation, Inc.,
#	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#include <sandix/compiler.h>
#include <asm/page.h>

##
# __PAGE_OFFSET = 0xC0000000
#define LOAD_OFFSET		__PAGE_OFFSET
#define	LOAD_PHYS_OFFSET	0x10000

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

		##
		# Read Mostly Data
		# Align to Level-1 Cache Line Size
		
		. = ALIGN(L1_CACHE_LINE_SIZE);
		*(.data..read_mostly)
		
		__edata = .;
	}
	
	##
	# Init data and text, can be freed later
	#
	
	. = ALIGN(PAGE_SIZE);
	.init : {
		__sinit = .;
		*(.init.text)
		*(.init.data)
		__einit = .;
	}

	. = ALIGN(PAGE_SIZE);
	.bss : {
		__bss_start__ = .;
		*(.bss)
		__bss_end__ = .;
	}

	/DISCARD/ : { *(.note*) }
	/DISCARD/ : { *(.eh_frame) }
	/DISCARD/ : { *(.comment) }
	
	__kend = .;
}
