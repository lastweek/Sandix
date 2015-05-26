/*
 *	2015/04/11. Created by Shan Yizhou.
 *	main.c: Real-Mode kernel main body.
 *
 *	This code asks the BIOS for memory/disk/other parameters, and
 *	puts them in a "safe" place. After necessary preparation, control
 *	will be transfered to protected-mode kernel.
 */

#include "boot.h"
#include <sandix/bootparam.h>

/**
 * bss section
 * &boot_params == __bss_start
 **/
struct boot_params boot_params;

/**
 * Query the keyboard lock status as given by the BIOS, and
 * set the keyboard repeat rate to maximum.  Unclear why the latter
 * is done here; this might be possible to kill off as stale code.
 **/
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
	keyboard_init();
	detect_memory();
	enable_a20();
	set_video();
	go_to_protected_mode();
}
