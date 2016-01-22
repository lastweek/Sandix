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

#include "boot.h"
#include "string.h"

#include <asm/bootparam.h>

/*
 * The "zeropage", used to store parameters acquired from BIOS.
 * It would be passed to kernel later when we enter protected mode.
 */
struct boot_params boot_params __attribute__((aligned(16)));

char *HEAP = __end;
char *heap_end = __end;	/* Default end of heap = no heap */

/*
 * Query the keyboard lock status as given by the BIOS, and
 * set the keyboard repeat rate to maximum.  Unclear why the latter
 * is done here; this might be possible to kill off as stale code.
 */
static void keyboard_init(void)
{
	struct biosregs ireg, oreg;
	initregs(&ireg);

	/* Get keyboard status */
	ireg.ah = 0x02;
	intcall(0x16, &ireg, &oreg);
	boot_params.kbd_status = oreg.al;

	/* Set keyboard repeat rate */
	ireg.ax = 0x0305;
	intcall(0x16, &ireg, NULL);
}

static void set_bios_mode(void)
{
#ifdef CONFIG_X86_64
	struct biosregs ireg;

	initregs(&ireg);
	ireg.ax = 0xec00;
	ireg.bx = 2;
	intcall(0x15, &ireg, NULL);
#endif
}

static void init_heap(void)
{
	char *stack_end;

	asm volatile (
		"nop\n\t"
		"nop\n\t"
		"leal %P1(%%esp), %0"
		: "=r" (stack_end)
		: "i" (-STACK_SIZE)
	);

	printf("%x\n", stack_end);

	heap_end = (char *)((unsigned long)boot_params.hdr.heap_end_ptr + 0x200);
	printf("%x\n", heap_end);

	if (heap_end > stack_end)
		heap_end = stack_end;
}

void main(void)
{
	/*
	 * copy header info from "header.S" to "zeropage"
	 */
	memcpy(&boot_params.hdr, &hdr, sizeof(struct setup_header));

	/* end of heap check */
	init_heap();

	/*
	 * tell BIOS what mode we gonna use (why?)
	 */
	set_bios_mode();

	/*
	 * detect physical memory (e820 memory table)
	 */
	detect_memory();
	
	/*
	 * set keyboard repear rate and query the lock flags (why?)
	 */
	keyboard_init();

	enable_a20();

	set_video();

	go_to_protected_mode();
}
