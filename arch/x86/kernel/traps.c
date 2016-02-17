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

#include <asm/traps.h>

#include <sandix/kdebug.h>
#include <sandix/kernel.h>
#include <sandix/signal.h>
#include <sandix/ptrace.h>
#include <sandix/compiler.h>

static void do_error_trap(struct pt_regs *regs, long error_code, char *str,
			  unsigned long trapnr, int signr)
{
	printk(KERN_INFO "%s, trapnr=%lu\n", str, trapnr);
	show_regs(regs);
}

#define DO_ERROR_TRAP(str, name, trapnr, signr)				\
dotraplinkage void do_##name(struct pt_regs *regs, long error_code)	\
{									\
	do_error_trap(regs, error_code, str, trapnr, signr);		\
}

DO_ERROR_TRAP("divide error",		     divide_error,		  X86_TRAP_DE,	   SIGFPE  )
DO_ERROR_TRAP("debug",			     debug,			  X86_TRAP_DB,	   SIGTRAP )
DO_ERROR_TRAP("int3",			     int3,			  X86_TRAP_BP,	   SIGTRAP )
DO_ERROR_TRAP("overflow",     		     overflow,			  X86_TRAP_OF,	   SIGSEGV )
DO_ERROR_TRAP("bounds",			     bounds,			  X86_TRAP_BR,	   SIGSEGV )
DO_ERROR_TRAP("invalid opcode",		     invalid_op,		  X86_TRAP_UD,	   SIGILL  )
DO_ERROR_TRAP("invalid TSS",		     invalid_TSS,		  X86_TRAP_TS,	   SIGSEGV )
DO_ERROR_TRAP("segment not present",	     segment_not_present,	  X86_TRAP_NP,	   SIGBUS  )
DO_ERROR_TRAP("stack segment",		     stack_segment,		  X86_TRAP_SS,	   SIGBUS  )
DO_ERROR_TRAP("alignment check",	     alignment_check,		  X86_TRAP_AC,	   SIGBUS  )
DO_ERROR_TRAP("simd exception",		     simd_exception,		  X86_TRAP_XM,	   SIGSEGV )
DO_ERROR_TRAP("coprocessor segment overrun", coprocessor_segment_overrun, X86_TRAP_OLD_MF, SIGFPE  )

dotraplinkage void do_nmi(struct pt_regs *regs, long error_code)
{

}

dotraplinkage void do_device_not_available(struct pt_regs *regs, long error_code)
{

}

dotraplinkage void do_double_fault(struct pt_regs *regs, long error_code)
{

}

dotraplinkage void do_general_protection(struct pt_regs *regs, long error_code)
{

}

dotraplinkage void do_page_fault(struct pt_regs *regs, long error_code)
{

}

dotraplinkage void do_machine_check(struct pt_regs *regs, long error_code)
{

}

dotraplinkage void do_virtualization_exception(struct pt_regs *regs, long error_code)
{

}

dotraplinkage void do_reserved(struct pt_regs *regs, long error_code)
{

}

void __init traps_init(void)
{

}
