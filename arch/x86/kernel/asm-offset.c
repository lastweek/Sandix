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

#include <sandix/kbuild.h>
#include <sandix/stddef.h>
#include <sandix/ptrace.h>

#include <asm/bootparam.h>

#ifdef CONFIG_X86_32
# include "asm-offset_32.c"
#else
# include "asm-offset_64.c"
#endif

void common(void)
{
	BLANK();
	OFFSET(BP_loadflags, boot_params, hdr.loadflags);
	OFFSET(BP_version, boot_params, hdr.version);
	OFFSET(BP_code32_start, boot_params, hdr.code32_start);

	BLANK();
	DEFINE(PTREGS_SIZE, sizeof(struct pt_regs));
}
