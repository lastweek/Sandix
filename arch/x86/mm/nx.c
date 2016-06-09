/*
 *	Copyright (C) 2016 Yizhou Shan <shan13@purdue.edu>
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

#include <asm/pgtable.h>
#include <asm/processor.h>
#include <asm/cpufeature.h>

#include <sandix/kernel.h>

void __init x86_configure_nx(void)
{
	if (cpu_has_nx)
		__supported_pte_mask |= __PAGE_NX;
	else
		__supported_pte_mask &= ~__PAGE_NX;
}

void __init x86_report_nx(void)
{
	if (!cpu_has_nx) {
		printk(KERN_NOTICE "Notice: NX (Execute Disable) protection "
				   "missing in CPU!\n");
	} else {
		/*
		 * Only 64-bit kernel or 32-bit with PAE enabled kernel
		 * will support __PAGE_NX bit. Check pgtable_types.h if
		 * need this feature within Sandix kernel.
		 */
#if defined(CONFIG_X86_64) || defined (CONFIG_X86_PAE)
		printk(KERN_INFO "NX (Execute Disable) protection: "
				 "active\n");
#else
		/* 32bit non-PAE kernel, NX cannot be used */
		printk(KERN_NOTICE "Notice: NX (Execute Disable) protection "
				   "cannot be enabled: non-PAE kernel!\n");
#endif
	}
}
