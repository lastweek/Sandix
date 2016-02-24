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

#include <asm/msr.h>
#include <asm/processor.h>
#include <asm/cpufeature.h>

#include <sandix/kernel.h>
#include <sandix/printk.h>

static void intel_cpu_early_init(struct cpuinfo_x86 *c)
{
	unsigned long long misc_enable;

#ifdef CONFIG_X86_64
	cpu_set_cap(c, X86_FEATURE_SYSENTER32);
#else
	/* Netburst reports 64 bytes clflush size, but does IO in 128 bytes */
	if (c->x86 == 0xf && c->x86_cache_alignment == 64)
		c->x86_cache_alignment = 128;
#endif

	/* CPUID workaround for 0F33/0F34 CPU */
	if (c->x86 == 0xF && c->x86_model == 0x3
	    && (c->x86_mask == 0x3 || c->x86_mask == 0x4))
		c->x86_phys_bits = 36;

	/*
	 * There is a known erratum on Pentium III and Core Solo
	 * and Core Duo CPUs.
	 * " Page with PAT set to WC while associated MTRR is UC
	 *   may consolidate to UC "
	 * Because of this erratum, it is better to stick with
	 * setting WC in MTRR rather than using PAT on these CPUs.
	 *
	 * Enable PAT WC only on P4, Core 2 or later CPUs.
	 */
	if (c->x86 == 6 && c->x86_model < 15)
		cpu_clear_cap(c, X86_FEATURE_PAT);

	/*
	 * If fast string is not enabled in IA32_MISC_ENABLE for any reason,
	 * clear the fast string and enhanced fast string CPU capabilities.
	 */
	if (c->x86 > 6 || (c->x86 == 6 && c->x86_model >= 0xd)) {
		rdmsrl(MSR_IA32_MISC_ENABLE, misc_enable);
		if (!(misc_enable & MSR_IA32_MISC_ENABLE_FAST_STRING)) {
			printk(KERN_INFO "Disabled fast string operations\n");
			cpu_clear_cap(c, X86_FEATURE_REP_GOOD);
			cpu_clear_cap(c, X86_FEATURE_ERMS);
		}
	}

	/*
	 * Intel Quark Core DevMan_001.pdf section 6.4.11
	 * "The operating system also is required to invalidate (i.e., flush)
	 *  the TLB when any changes are made to any of the page table entries.
	 *  The operating system must reload CR3 to cause the TLB to be flushed"
	 *
	 * As a result cpu_has_pge() in arch/x86/include/asm/tlbflush.h should
	 * be false so that __flush_tlb_all() causes CR3 insted of CR4.PGE
	 * to be modified
	 */
	if (c->x86 == 5 && c->x86_model == 9) {
		pr_info("Disabling PGE capability bit\n");
		cpu_clear_cap(c, X86_FEATURE_PGE);
	}
}

struct cpu_dev intel_cpu_dev = {
	.cpu_vendor	= "Intel",
	.cpu_ident	= "GenuineIntel",
	.cpu_x86_vendor	= X86_VENDOR_INTEL,
	.cpu_early_init	= intel_cpu_early_init
};
