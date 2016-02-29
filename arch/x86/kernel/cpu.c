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

#include <asm/asm.h>
#include <asm/processor.h>
#include <asm/cpufeature.h>

#include <sandix/ctype.h>
#include <sandix/types.h>
#include <sandix/string.h>
#include <sandix/export.h>
#include <sandix/kernel.h>
#include <sandix/printk.h>

struct cpuinfo_x86 boot_cpu_info __read_mostly;
EXPORT_SYMBOL(boot_cpu_info);

/* x86 vendor driver table */
static struct cpu_dev *cpudevs[] = {
	[X86_VENDOR_INTEL]	= &intel_cpu_dev,
	[X86_VENDOR_UNKNOWN]	= NULL
};

static struct cpu_dev dummy_cpu_dev __initdata = {
	.cpu_vendor	= "Unkonwn",
	.cpu_x86_vendor	= X86_VENDOR_UNKNOWN
};

static struct cpu_dev *this_cpu = &dummy_cpu_dev;

/* Get human-readable model string */
static void __init get_model_name(struct cpuinfo_x86 *c)
{
	unsigned int *v;
	char *p, *q, *s;

	if (c->extended_cpuid_level < 0x80000004)
		return;

	v = (unsigned int *)c->x86_model_id;
	cpuid(0x80000002, &v[0], &v[1], &v[2], &v[3]);
	cpuid(0x80000003, &v[4], &v[5], &v[6], &v[7]);
	cpuid(0x80000004, &v[8], &v[9], &v[10], &v[11]);
	c->x86_model_id[48] = 0;

	/* trim whitespace */
	p = q = s = &c->x86_model_id[0];

	while (*p == ' ')
		p++;

	while (*p) {
		/* note the last non-whitespace index */
		if (!isspace(*p))
			s = q;

		*q++ = *p++;
	}

	*(s + 1) = '\0';
}

static void __init get_cpu_basic(struct cpuinfo_x86 *c)
{
	/* get vendor string id */
	cpuid(0x00000000,
	      (unsigned int *)&c->cpuid_level,
	      (unsigned int *)&c->x86_vendor_id[0],
	      (unsigned int *)&c->x86_vendor_id[8],
	      (unsigned int *)&c->x86_vendor_id[4]);

	/* get basic cpu information */
	c->x86 = 4;
	if (c->cpuid_level > 0x00000001) {
		unsigned int eax, ebx, ecx, edx;

		cpuid(0x00000001, &eax, &ebx, &ecx, &edx);

		/* family, model, stepping id */
		c->x86_mask = eax & 0xf;
		c->x86_model = (eax >> 4) & 0xf;
		c->x86 = (eax >> 8) & 0xf;

		if (c->x86 == 0xf) {
			c->x86 += (eax >> 20) & 0xff;
			c->x86_model += ((eax >> 16) & 0xf) << 4;
		}

		if (c->x86 == 0x6)
			c->x86_model += ((eax >> 16) & 0xf) << 4;

		/* cache line size */
		if (edx & (1 << 19)) {
			c->x86_clflush_size = ((ebx >> 8) & 0xff) * 8;
			c->x86_cache_alignment = c->x86_clflush_size;
		}
	}
}

static void __init get_cpu_capability(struct cpuinfo_x86 *c)
{
	unsigned int eax, ebx, ecx, edx;

	/* Basic CPUID Information */
	if (c->cpuid_level >= 0x00000001) {
		cpuid(0x00000001, &eax, &ebx, &ecx, &edx);
		c->x86_capability[0] = edx;
		c->x86_capability[4] = ecx;
	}

	/* Structured Extended Feature Flags Enumeration Leaf  */
	if (c->cpuid_level >= 0x00000007) {
		cpuid_count(0x00000007, 0, &eax, &ebx, &ecx, &edx);
		c->x86_capability[9] = ebx;
	}

	/* Processor Extended State Enumeration Sub-leaf  */
	if (c->cpuid_level >= 0x0000000d) {
		cpuid_count(0x0000000d, 1, &eax, &ebx, &ecx, &edx);
		c->x86_capability[10] = eax;
	}

	/* Platform QoS Monitoring Enumeration Sub-leaf */
	if (c->cpuid_level >= 0x0000000f) {
		/* QoS sub-leaf, eax=0fh, ecx=0 */
		cpuid_count(0x0000000f, 0, &eax, &ebx, &ecx, &edx);
		c->x86_capability[11] = edx;
		if (cpu_has_cap(c, X86_FEATURE_CQM_LLC)) {
			c->x86_cache_max_rmid = ebx;
			/* QoS sub-leaf, eax=0fh, ecx=1 */
			cpuid_count(0x0000000f, 1, &eax, &ebx, &ecx, &edx);
			c->x86_capability[12] = edx;
			if (cpu_has_cap(c, X86_FEATURE_CQM_OCCUP_LLC)) {
				c->x86_cache_max_rmid = ecx;
				c->x86_cache_occ_scale = ebx;
			}
		} else {
			c->x86_cache_max_rmid = -1;
			c->x86_cache_occ_scale = -1;
		}
	}

	/* Maximum Input Value for Extended CPUID */
	eax = cpuid_eax(0x80000000);
	c->extended_cpuid_level = eax;

	/* Extended Processor Signature and Feature Bits */
	if (c->extended_cpuid_level >= 0x80000001) {
		cpuid(0x80000001, &eax, &ebx, &ecx, &edx);
		c->x86_capability[1] = edx;
		c->x86_capability[6] = ecx;
	}

	/* Virtual/Physical Address Size */
	if (c->extended_cpuid_level >= 0x80000008) {
		cpuid(0x80000008, &eax, &ebx, &ecx, &edx);
		c->x86_phys_bits = eax & 0xff;
		c->x86_virt_bits = (eax >> 8) & 0xff;
		c->x86_capability[13] = ebx;
	}
}

static void __init identify_cpu_vendor(struct cpuinfo_x86 *c)
{
	int i;
	char *v = c->x86_vendor_id;

	for (i = 0; i < X86_VENDOR_UNKNOWN; i++) {
		if (!cpudevs[i])
			break;
		/*
		 * Check if the vendor string obtained from local cpu
		 * matchs the vendor string provided by vendor driver.
		 */
		if (!strcmp(v, cpudevs[i]->cpu_ident)) {
			this_cpu = cpudevs[i];
			c->x86_vendor = this_cpu->cpu_x86_vendor;
			return;
		}
	}
	panic("CPU: vendor_id '%s' unknown, we only support Intel CPU", v);
}

static void __init early_identify_cpu(struct cpuinfo_x86 *c)
{
#ifdef CONFIG_X86_32
	c->x86_clflush_size = 32;
	c->x86_virt_bits = 32;
	c->x86_phys_bits = 32;
#else
	c->x86_clflush_size = 64;
	c->x86_virt_bits = 48;
	c->x86_phys_bits = 36;
#endif
	c->x86_cache_alignment = c->x86_clflush_size;
	c->cpu_index = 0;

	/* get basic information first */
	get_cpu_basic(c);

	/* then see if we are running upon intel */
	identify_cpu_vendor(c);

	/* if yes, detect its capability */
	get_cpu_capability(c);

	get_model_name(c);

	if (this_cpu->cpu_early_init)
		this_cpu->cpu_early_init(c);

	if (this_cpu->cpu_bsp_init)
		this_cpu->cpu_bsp_init(c);
}

/*
 * Do minimum CPU detection early. Fields really needed: vendor, cpuid_level,
 * family, model, mask, cache alignment. The others are not touched to avoid
 * unwanted side effects.
 *
 * WARNING: this function is only called on the BP.
 * Don't add code here that is supposed to run on all CPUs.
 */
void __init early_cpu_init(void)
{
	int i = 0;
#ifdef CONFIG_PRINT_SUPPORTED_CPUS
	printk(KERN_INFO "Kernel supported CPUs:\n");
	while (cpudevs[i++]) {
		struct cpu_dev *cdev = cpudevs[i - 1];
		printk(KERN_INFO "  %s %s\n", cdev->cpu_vendor, cdev->cpu_ident);
	}
#endif
	early_identify_cpu(&boot_cpu_info);
}

void __init cpu_init(void)
{

}

__used
void print_cpu_info(struct cpuinfo_x86 *c)
{
	if (c->x86_model_id[0])
		printk(KERN_CONT "%s", c->x86_model_id);
	else
		printk(KERN_CONT "%d86", c->x86);

	printk(KERN_CONT " (family: 0x%x, model: 0x%x", c->x86, c->x86_model);

	if (c->x86_mask || c->cpuid_level >= 0)
		printk(KERN_CONT ", stepping: 0x%x)\n", c->x86_mask);
	else
		printk(KERN_CONT ")\n");
}
