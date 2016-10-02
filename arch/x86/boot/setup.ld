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

/*
 * This file describes linker script for linking kernel setup part.
 * Several symbols are used by assembly and C source code.
 */

OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)

SECTIONS
{
	. = 0;
	.bstext		: { *(.bstext) }
	.bsdata		: { *(.bsdata) }

	. = 495;
	.header		: { *(.header) }
	.entrytext	: { *(.entrytext) }
	.entrydata	: { *(.entrydata) }

	.text		: { *(.text) }
	.text32		: { *(.text32) }
	
	. = ALIGN(16);
	.rodata		: { *(.rodata*) }
	
	.videocards	: {
		video_cards = .;
		*(.videocards)
		video_cards_end = .;
	}

	. = ALIGN(16);
	.data		: { *(.data*) }
	
	.signature	: {
		setup_sig = .;
		LONG(0x5a5aaa55)
	}

	. = ALIGN(16);
	.bss		: {
		__bss_start = .;
		*(.bss)
		__bss_end = .;
	}

	. = ALIGN(16);
	__end = .;
	
	/DISCARD/ : { *(.note*) }
	/DISCARD/ : { *(.eh_frame) }
	/DISCARD/ : { *(.comment) }
	
	/* Confine the setup to 64 sectors(63 exactly) to avoid cross cylinder */
	. = ASSERT(__end <= 0x8000, "Setup too big!");
	. = ASSERT(hdr == 0x1f1, "The setup header has the wrong offset!");
}
