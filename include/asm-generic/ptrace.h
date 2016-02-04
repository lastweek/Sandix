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
 * Common low level (register) ptrace helpers
 */

#ifndef _ASM_GENERIC_PTRACE_H_
#define _ASM_GENERIC_PTRACE_H_

/*
 * Helpers for working with instruction pointer
 */

#ifndef GET_IP
#define GET_IP(regs)		((regs)->pc)
#endif

#ifndef SET_IP
#define SET_IP(regs, val)	(GET_IP(regs) = (val))
#endif

static inline unsigned long instruction_pointer(struct pt_regs *regs)
{
	return GET_IP(regs);
}
static inline void instruction_pointer_set(struct pt_regs *regs,
                                           unsigned long val)
{
	SET_IP(regs, val);
}

#ifndef profile_pc
#define profile_pc(regs)	instruction_pointer(regs)
#endif

/*
 * Helpers for working with the user stack pointer
 */

#ifndef GET_USP
#define GET_USP(regs)		((regs)->usp)
#endif

#ifndef SET_USP
#define SET_USP(regs, val)	(GET_USP(regs) = (val))
#endif

static inline unsigned long user_stack_pointer(struct pt_regs *regs)
{
	return GET_USP(regs);
}
static inline void user_stack_pointer_set(struct pt_regs *regs,
                                          unsigned long val)
{
	SET_USP(regs, val);
}

/*
 * Helpers for working with the frame pointer
 */

#ifndef GET_FP
#define GET_FP(regs)		((regs)->fp)
#endif

#ifndef SET_FP
#define SET_FP(regs, val)	(GET_FP(regs) = (val))
#endif

static inline unsigned long frame_pointer(struct pt_regs *regs)
{
	return GET_FP(regs);
}
static inline void frame_pointer_set(struct pt_regs *regs,
                                     unsigned long val)
{
	SET_FP(regs, val);
}

#endif /* _ASM_GENERIC_PTRACE_H_ */
