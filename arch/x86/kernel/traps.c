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
#include <asm/segment.h>
#include <asm/descriptor.h>
#include <asm/irq_vectors.h>

#include <sandix/kdebug.h>
#include <sandix/kernel.h>
#include <sandix/signal.h>
#include <sandix/ptrace.h>
#include <sandix/bitops.h>

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
DO_ERROR_TRAP("simd exception",		     simd_exception,		  X86_TRAP_XF,	   SIGSEGV )
DO_ERROR_TRAP("coprocessor segment overrun", coprocessor_segment_overrun, X86_TRAP_OLD_MF, SIGFPE  )

dotraplinkage void do_general_protection(struct pt_regs *regs, long error_code)
{
	panic("#GP");
}

dotraplinkage void do_page_fault(struct pt_regs *regs, long error_code)
{
	printk(KERN_INFO "Error code: %x\n", error_code);
	show_regs(regs);
	panic("#PF");
}

dotraplinkage void do_nmi(struct pt_regs *regs, long error_code)
{
	panic("NMI");
}

dotraplinkage void do_device_not_available(struct pt_regs *regs, long error_code)
{
	panic("Device Not Avaliable");
}

dotraplinkage void do_double_fault(struct pt_regs *regs, long error_code)
{
	panic("Double Fault");
}

dotraplinkage void do_spurious_interrupt_bug(struct pt_regs *regs, long error_code)
{
	panic("Spurious Interrupt Bug");
}

dotraplinkage void do_coprocessor_error(struct pt_regs *regs, long error_code)
{
	panic("X87 Coprocessor Error");
}

dotraplinkage void do_machine_check(struct pt_regs *regs, long error_code)
{
	panic("Machine Check");
}

dotraplinkage void do_virtualization_exception(struct pt_regs *regs, long error_code)
{
	panic("Virtualization Exception");
}

dotraplinkage void do_reserved(struct pt_regs *regs, long error_code)
{
	panic("Reserved");
}

int first_system_vector = FIRST_SYSTEM_VECTOR;

DEFINE_BITMAP(used_vectors, NR_VECTORS);
EXPORT_SYMBOL(used_vectors);

void __init trap_init(void)
{
	int i;

	set_intr_gate(X86_TRAP_DE, divide_error);
	set_intr_gate(X86_TRAP_DB, debug);
	set_intr_gate(X86_TRAP_NMI, nmi);
	set_intr_gate(X86_TRAP_BP, int3);
	set_intr_gate(X86_TRAP_OF, overflow);
	set_intr_gate(X86_TRAP_BR, bounds);
	set_intr_gate(X86_TRAP_UD, invalid_op);
	set_intr_gate(X86_TRAP_NM, device_not_available);
	set_intr_gate(X86_TRAP_DF, double_fault);
	set_intr_gate(X86_TRAP_OLD_MF, coprocessor_segment_overrun);
	set_intr_gate(X86_TRAP_TS, invalid_TSS);
	set_intr_gate(X86_TRAP_NP, segment_not_present);
	set_intr_gate(X86_TRAP_SS, stack_segment);
	set_intr_gate(X86_TRAP_GP, general_protection);
	set_intr_gate(X86_TRAP_PF, page_fault);
	set_intr_gate(X86_TRAP_SPURIOUS, spurious_interrupt_bug);
	set_intr_gate(X86_TRAP_MF, coprocessor_error);
	set_intr_gate(X86_TRAP_AC, alignment_check);
	set_intr_gate(X86_TRAP_MC, machine_check);
	set_intr_gate(X86_TRAP_XF, simd_exception);
	set_intr_gate(X86_TRAP_VE, virtualization_exception);

	/* Reserve all predefined vectors */
	for (i = 0; i < FIRST_EXTERNAL_VECTOR; i++)
		set_bit(i, used_vectors);

	/*
	 * IA32 INT80 System Call
	 * XXX: intr or trap gate???
	 */
	set_system_intr_gate(SYSCALL_VECTOR, entry_INT80_32);
	set_bit(SYSCALL_VECTOR, used_vectors);
}
