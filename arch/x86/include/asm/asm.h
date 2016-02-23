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
 * Wrappers for some special assembly instructions.
 */

#ifndef _ASM_X86_ASM_H_
#define _ASM_X86_ASM_H_

#define LOCK_PREFIX "lock; "

static inline void __cpuid(unsigned int *eax, unsigned int *ebx,
			   unsigned int *ecx, unsigned int *edx)
{
	/* ecx is often an input as well as an output. */
	asm volatile (
		"cpuid"
		: "=a" (*eax), "=b" (*ebx),
		  "=c" (*ecx), "=d" (*edx)
		: "0"  (*eax), "2"  (*ecx)
		: "memory"
	);
}

/*
 * Generic CPUID function
 * clear %ecx since some cpus (Cyrix MII) do not set or clear %ecx
 * resulting in stale register contents being returned.
 */
static inline void cpuid(unsigned int op,
			 unsigned int *eax, unsigned int *ebx,
			 unsigned int *ecx, unsigned int *edx)
{
	*eax = op;
	*ecx = 0;
	__cpuid(eax, ebx, ecx, edx);
}

/* Some CPUID calls want 'count' to be placed in ecx */
static inline void cpuid_count(unsigned int op, int count,
			       unsigned int *eax, unsigned int *ebx,
			       unsigned int *ecx, unsigned int *edx)
{
	*eax = op;
	*ecx = count;
	__cpuid(eax, ebx, ecx, edx);
}

/* CPUID calls that only return one result */
#define BUILD_CPUID_XXX(reg)					\
static inline unsigned int cpuid_##reg(unsigned int op)		\
{								\
	unsigned int eax, ebx, ecx, edx;			\
	cpuid(op, &eax, &ebx, &ecx, &edx);			\
	return reg;						\
}
BUILD_CPUID_XXX(eax)
BUILD_CPUID_XXX(ebx)
BUILD_CPUID_XXX(ecx)
BUILD_CPUID_XXX(edx)

#define nop()		asm volatile ("nop");
#define rep_nop()	asm volatile ("rep; nop" ::: "memory");
#define cpu_relax()	rep_nop()

#endif /* _ASM_X86_ASM_H_ */
