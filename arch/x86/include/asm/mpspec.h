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

#ifndef _ASM_X86_MPSPEC_H_
#define _ASM_X86_MPSPEC_H_

#include <asm/x86_init.h>

static inline void get_smp_config(void)
{
	x86_init.mpparse.get_smp_config(0);
}

static inline void early_get_smp_config(void)
{
	x86_init.mpparse.get_smp_config(1);
}

static inline void find_smp_config(void)
{
	x86_init.mpparse.find_smp_config();
}

#endif /* _ASM_X86_MPSPEC_H_ */
