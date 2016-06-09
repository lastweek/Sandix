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

#include <asm/setup.h>
#include <asm/x86_init.h>

#include <sandix/compiler.h>

void x86_init_noop(void) { }
void x86_init_uint_noop(unsigned int unused) { }

struct x86_init_ops x86_init __initdata = {
	.resources = {
		.probe_roms		= probe_roms,
	},

	.oem = {
		.arch_setup		= x86_init_noop,
		.banner			= x86_init_noop,
	},
};
