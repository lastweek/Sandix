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

#ifndef _ASM_X86_E820_H_
#define _ASM_X86_E820_H_

/* e820 memory segment type */
#define E820_RAM_TYPE		1		/* Available RAM */
#define E820_RESERVED_TYPE	2		/* Reserved */
#define E820_ACPI_TYPE		3
#define E820_NVS_TYPE		4
#define E820_UNUSABLE_TYPE	5

/* Conventional segment start address */
#define ISA_START_ADDRESS	0xa0000
#define ISA_END_ADDRESS		0x100000
#define BIOS_BEGIN		0x000a0000
#define BIOS_END		0x00100000
#define BIOS_ROM_BASE		0xffe00000
#define BIOS_ROM_END		0xffffffff

/* Max e820 entries */
#define E820MAX			128

#ifndef __ASSEMBLY__

/* e820entry structure, returned by BIOS */
struct e820entry {
	unsigned long long	addr;	/* start of memory segment */
	unsigned long long	size;	/* size of memory segment */
	unsigned int		type;	/* type of memory segment */
} __attribute__((packed));

struct e820map {
	unsigned int nr_entries;
	struct e820entry map[E820MAX];
}__attribute__((packed));

#endif /* __ASSEMBLY__ */
#endif /* _ASM_X86_E820_H_ */
