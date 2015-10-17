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

#include <sandix/kernel.h>
#include <sandix/types.h>
#include <sandix/tty.h>
#include <sandix/printk.h>

#include <asm/setup.h>

void handle_int(void)
{
	asm volatile (
		"incb 0xb8000\n\t"
		"incb 0xb8001\n\t"
		"iret"
	);
}

void hlt(void)
{
	asm volatile (
		"1: hlt\n\t"
		"jmp 1b"
	);
}

asmlinkage void __init start_kernel(void)
{
	int i=5;
	arch_setup();
	tty_init();
	
	printk("\033[035m%s\033[0m\n\r", sandix_banner);
	
	panic("yes %d", 2008);
	panic("yes %d", 2008);
	hlt();
}
