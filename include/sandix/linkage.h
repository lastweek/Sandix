/*
 *	linkage.h - Generic macros for assembly and C attributes.
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
 *
 */

#ifndef _SANDIX_LINKAGE_H_
#define _SANDIX_LINKAGE_H_

/* Pass parameters from stack */
#define asmlinkage __atrribute__((regparm(0)))

/* GNU AS newline separator */
#define	ASM_NL	;

/* Pad with NOP in x86 */
#ifndef ALIGN
#define ALIGN \
	.align 4, 0x90
#endif

#ifndef WEAK
#define WEAK(name) \
	.weak name ASM_NL \
	name:
#endif

#ifndef GLOBAL
#define GLOBAL(name) \
	.globl name ASM_NL \
	name:
#endif

#ifndef ENTRY
#define ENTRY(name) \
	.globl name ASM_NL \
	ALIGN ASM_NL \
	name:
#endif

#ifndef END
#define END(name) \
	.size name, .-name
#endif

/*
 * If symbol 'name' is treated as a subroutine (gets called, and returns)
 * then please use ENDPROC to mark 'name' as STT_FUNC.(For future use...)
 */
#ifndef ENDPROC
#define ENDPROC(name) \
	.type name, @function ASM_NL \
	END(name)
#endif

#endif /* _SANDIX_LINKAGE_H_ */
