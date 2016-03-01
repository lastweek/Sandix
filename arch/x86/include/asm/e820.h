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

#ifndef _ASM_X86_E820_H_
#define _ASM_X86_E820_H_

#include <sandix/compiler.h>

/*
 * Max e820 entries
 */
#define E820MAX			128

/*
 * e820 memory segment type
 */
#define E820_RAM		1		/* Available RAM */
#define E820_RESERVED		2		/* Reserved */
#define E820_ACPI		3
#define E820_NVS		4
#define E820_UNUSABLE		5
#define E820_PMEM		7		/* Persistent Memory */

/*
 * This is a non-standardized way to represent ADR or NVDIMM regions that
 * persist over a reboot.  The kernel will ignore their special capabilities
 * unless the CONFIG_X86_PMEM_LEGACY=y option is set.
 *
 * ( Note that older platforms also used 6 for the same type of memory,
 *   but newer versions switched to 12 as 6 was assigned differently.  Some
 *   time they will learn... )
 */
#define E820_PRAM		12

/*
 * reserved RAM used by kernel itself
 * if CONFIG_INTEL_TXT is enabled, memory of this type will be
 * included in the S3 integrity calculation and so should not include
 * any memory that BIOS might alter over the S3 transition
 */
#define E820_RESERVED_KERN	128

#define ISA_START_ADDRESS	0xa0000
#define ISA_END_ADDRESS		0x100000

#define BIOS_BEGIN		0x000a0000
#define BIOS_END		0x00100000

#define BIOS_ROM_BASE		0xffe00000
#define BIOS_ROM_END		0xffffffff

#ifndef __ASSEMBLY__

/* e820entry structure, returned by BIOS */
struct e820entry {
	unsigned long long	addr;	/* start of memory segment */
	unsigned long long	size;	/* size of memory segment */
	unsigned int		type;	/* type of memory segment */
} __attribute__((packed));

/* e820 map used by kernel */
struct e820map {
	unsigned int nr_entries;
	struct e820entry map[E820MAX];
}__attribute__((packed));

unsigned long __init e820_end_of_low_ram_pfn(void);
unsigned long __init e820_end_of_ram_pfn(void);
int __init sanitize_e820_map(struct e820entry *biosmap, int max_nr_map, u32 *pnr_map);
void __init e820_add_region(u64 start, u64 size, u32 type);
void __init e820_print_map(char *who);
void __init setup_memory_map(void);

#endif /* __ASSEMBLY__ */
#endif /* _ASM_X86_E820_H_ */
