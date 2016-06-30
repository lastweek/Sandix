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

#include <asm/asm.h>
#include <asm/page.h>
#include <asm/traps.h>
#include <asm/processor.h>

#include <sandix/mm.h>
#include <sandix/kernel.h>
#include <sandix/ptrace.h>
#include <sandix/kdebug.h>

/*
 * Page fault error code bits:
 *
 *   bit 0 ==	 0: no page found	1: protection fault
 *   bit 1 ==	 0: read access		1: write access
 *   bit 2 ==	 0: kernel-mode access	1: user-mode access
 *   bit 3 ==				1: use of reserved bit detected
 *   bit 4 ==				1: fault was an instruction fetch
 *   bit 5 ==				1: protection keys block access
 */
enum x86_pf_error_code {

	PF_PROT		=		1 << 0,
	PF_WRITE	=		1 << 1,
	PF_USER		=		1 << 2,
	PF_RSVD		=		1 << 3,
	PF_INSTR	=		1 << 4,
	PF_PK		=		1 << 5,
};

static inline int fault_in_kernel_space(unsigned long address)
{
	return address >= TASK_SIZE_MAX;
}

/*
 * This routine handles page faults. It determines the address,
 * and the problem, and then passes it off to one of the appropriate
 * routines.
 */
static void __do_page_fault(struct pt_regs *regs, unsigned long error_code,
			    unsigned long address)
{
	/*
	 * We fault-in kernel-space virtual memory on-demand. The
	 * 'reference' page table is init_mm.pgd.
	 *
	 * NOTE! We MUST NOT take any locks for this case. We may
	 * be in an interrupt or a critical region, and should
	 * only copy the information from the master page table,
	 * nothing more.
	 *
	 * This verifies that the fault happens in kernel space
	 * (error_code & 4) == 0, and that the fault was not a
	 * protection error (error_code & 9) == 0.
	 */
	if (unlikely(fault_in_kernel_space(address))) {
	}
}

dotraplinkage void do_page_fault(struct pt_regs *regs, long error_code)
{
	unsigned long address = read_cr2();

	__do_page_fault(regs, error_code, address);

	printk("BUG: unable to handle kernel paging request at %#lx\n", address);
	show_regs(regs);
	panic("panic at #PF");
}
