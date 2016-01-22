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

/*
 * Ask BIOS for physical memory map
 */

#include "boot.h"
#include <asm/e820.h>
#include <asm/bootparam.h>

/* ASICC "SWAP" */
#define SMAP	0x534D4150

static int detect_memory_e820(void)
{
	int count = 0;
	struct biosregs ireg, oreg;
	struct e820entry *desc = boot_params.e820_map;
	struct e820entry buf = {0,0,0};

	initregs(&ireg);
	ireg.ax  = 0xe820;
	ireg.cx  = sizeof(struct e820entry);
	ireg.edx = SMAP;
	ireg.di  = (size_t)&buf;

	/*
	 * Note: at least one BIOS is known which assumes that the
	 * buffer pointed to by one e820 call is the same one as
	 * the previous call, and only changes modified fields.  Therefore,
	 * we use a temporary buffer and copy the results entry by entry.
	 *
	 * This routine deliberately does not try to account for
	 * ACPI 3+ extended attributes.  This is because there are
	 * BIOSes in the field which report zero for the valid bit for
	 * all ranges, and we don't currently make any use of the
	 * other attribute bits.  Revisit this if we see the extended
	 * attribute bits deployed in a meaningful way in the future.
	 */

	do {
		intcall(0x15, &ireg, &oreg);
		ireg.ebx = oreg.ebx; /* for next iteration */

		/*
		 * BIOSes which terminate the chain with CF = 1 as opposed
		 * to %ebx = 0 don't always report the SMAP signature on
		 * the final, failing, probe. (1 means X86_EFLAGS_CF)
		 */
		if (oreg.eflags & 1)
			break;

		/*
		 * Some BIOSes stop returning SMAP in the middle of
		 * the search loop.  We don't know exactly how the BIOS
		 * screwed up the map at that point, we might have a
		 * partial map, the full map, or complete garbage, so
		 * just return failure.
		 */
		if (oreg.eax != SMAP) {
			count = 0;
			break;
		}
		
		*desc++ = buf;
		count++;

	} while (ireg.ebx && count < E820MAX);
	
	return boot_params.e820_nr_entries = count;
}

void detect_memory(void)
{
	if (!detect_memory_e820())
		puts("Detect physical memory failed...\n");
}
