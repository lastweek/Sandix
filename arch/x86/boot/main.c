/*
 *	arch/x86/boot/main.c - Real-Mode Setup Part Kernel
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

/*
 *	This code asks the BIOS for memory/disk/other parameters, and puts them
 *	in a "safe" place. After necessary preparation, control will be
 *	transfered to protected-mode kernel.
 */

#include "boot.h"
#include <asm/bootparam.h>

/*
 * Sleep in bss section:
 * &boot_params == __bss_start
 */
struct boot_params boot_params;

/*
 * Query the keyboard lock status as given by the BIOS, and
 * set the keyboard repeat rate to maximum.  Unclear why the latter
 * is done here; this might be possible to kill off as stale code.
 */
static void keyboard_init(void)
{
	struct biosregs ireg, oreg;
	initregs(&ireg);

	ireg.ah = 0x02;		// Get keyboard status
	intcall(0x16, &ireg, &oreg);
	boot_params.kbd_status = oreg.al;

	ireg.ax = 0x0305;	// Set keyboard repeat rate
	intcall(0x16, &ireg, NULL);
}

void main(void)
{
	/* Copy setup_header in header.S to boot_params */
	boot_params.hdr = hdr;

	/* Various setup routines */
	keyboard_init();
	detect_memory();
	enable_a20();
	
	/* Don't print anything after set_video() */
	/* Because we have stored cursor position etc., already */
	set_video();
	go_to_protected_mode();
}
