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

#ifndef _ASM_X86_MSR_H_
#define _ASM_X86_MSR_H_

#include <sandix/types.h>
#include <sandix/bitops.h>

/*
 * both i386 and x86_64 returns 64-bit value in edx:eax, but gcc's "A" constraint
 * has different meanings. For i386, "A" means exactly edx:eax, while for x86_64
 * it doesn't mean rdx:rax or edx:eax. Instead, it means rax *or* rdx. See:
 * [https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints]
 */
#ifdef CONFIG_X86_32
#define DECLARE_ARGS(val, low, high)	unsigned long long val
#define EAX_EDX_VAL(val, low, high)	(val)
#define EAX_EDX_RET(val, low, high)	"=A" (val)
#else
#define DECLARE_ARGS(val, low, high)	unsigned long low, high
#define EAX_EDX_VAL(val, low, high)	((low) | (high) << 32)
#define EAX_EDX_RET(val, low, high)	"=a" (low), "=d" (high)
#endif

static inline unsigned long long native_read_msr(unsigned int msr)
{
	DECLARE_ARGS(val, low, high);
	asm volatile (
		"rdmsr"
		: EAX_EDX_RET(val, low, high)
		: "c" (msr)
	);
	return EAX_EDX_VAL(val, low, high);
}

static inline void native_write_msr(unsigned int msr, unsigned int low,
				    unsigned int high)
{
	asm volatile (
		"wrmsr"
		:
		: "c" (msr), "a" (low), "d" (high)
		: "memory"
	);
}

/**
 * rdtsc() - returns the current TSC without ordering constraints
 *
 * rdtsc() returns the result of RDTSC as a 64-bit integer.  The
 * only ordering constraint it supplies is the ordering implied by
 * "asm volatile": it will put the RDTSC in the place you expect.  The
 * CPU can and will speculatively execute that RDTSC, though, so the
 * results can be non-monotonic if compared on different CPUs.
 */
static inline unsigned long long rdtsc(void)
{
	DECLARE_ARGS(val, low, high);
	asm volatile (
		"rdtsc"
		: EAX_EDX_RET(val, low, high)
	);
	return EAX_EDX_VAL(val, low, high);
}

static inline unsigned long long native_read_pmc(int counter)
{
	DECLARE_ARGS(val, low, high);
	asm volatile (
		"rdpmc"
		: EAX_EDX_RET(val, low, high)
		: "c" (counter)
	);
	return EAX_EDX_VAL(val, low, high);
}

/*
 * Access to Machine Specific Register (MSR)
 *
 * NOTE: The rd* operations modify the parameters directly without
 * using pointer indirection, this allows gcc to optimize better.
 */

/*
 * WARNING: No exception handling
 */

#define rdmsr(msr, low, high)					\
do {								\
	unsigned long long __val = native_read_msr((msr));	\
	(low) = (unsigned int)__val;				\
	(high) = (unsigned int)(__val >> 32);			\
} while (0)

#define rdmsrl(msr, val)					\
do {								\
	(val) = native_read_msr((msr));				\
} while (0)

#define rdpmc(counter, low, high)				\
do {								\
	unsigned long long __val = native_read_pmc((counter));	\
	(low)  = (unsigned int)__val;				\
	(high) = (unsigned int)(__val >> 32);			\
} while (0)

#define rdpmcl(counter, val)					\
do {								\
	(val) = native_read_pmc(counter);			\
while (0)

static inline void wrmsr(unsigned int msr, unsigned int low, unsigned int high)
{
	native_write_msr(msr, low, high);
}

static inline void wrmsrl(unsigned int msr, unsigned long long val)
{
	native_write_msr(msr, (unsigned int)val, (unsigned int)(val >> 32));
}

/*
 * WARNING: No exception handling again
 */

static inline int msr_test_bit(unsigned int msr, unsigned int bit)
{
	unsigned long long val;
	rdmsrl(msr, val);
	return test_bit(bit, (unsigned long *)&val);
}

static inline void msr_set_bit(unsigned int msr, unsigned int bit)
{
	unsigned long long val;

	rdmsrl(msr, val);
	if (!test_bit(bit, (unsigned long *)&val)) {
		set_bit(bit, (unsigned long *)&val);
		wrmsrl(msr, val);
	}
}

static inline void msr_clear_bit(unsigned int msr, unsigned int bit)
{
	unsigned long long val;

	rdmsrl(msr, val);
	if (test_bit(bit, (unsigned long *)&val)) {
		clear_bit(bit, (unsigned long *)&val);
		wrmsrl(msr, val);
	}
}

/*
 * Intel defined MSRs
 */

#define MSR_IA32_P5_MC_ADDR			0x00000000
#define MSR_IA32_P5_MC_TYPE			0x00000001
#define MSR_IA32_TSC				0x00000010
#define MSR_IA32_PLATFORM_ID			0x00000017
#define MSR_IA32_APIC_BASE			0x0000001b
  #define MSR_IA32_APIC_BASE_BSP_BIT			8
  #define MSR_IA32_APIC_BASE_BSP			(1ULL<<MSR_IA32_APIC_BASE_BSP_BIT)
  #define MSR_IA32_APIC_BASE_ENABLE_BIT			11
  #define MSR_IA32_APIC_BASE_ENABLE			(1ULL<<MSR_IA32_APIC_BASE_ENABLE_BIT)
  #define MSR_IA32_APIC_BASE_BASE			(0xfffff<<12)
#define MSR_IA32_FEATURE_CONTROL		0x0000003a
#define MSR_IA32_TSC_ADJUST			0x0000003b
#define MSR_IA32_MISC_ENABLE			0x000001a0
  /* MISC_ENABLE bits: architectural */
  #define MSR_IA32_MISC_ENABLE_FAST_STRING_BIT		0
  #define MSR_IA32_MISC_ENABLE_FAST_STRING		(1ULL << MSR_IA32_MISC_ENABLE_FAST_STRING_BIT)
  #define MSR_IA32_MISC_ENABLE_TCC_BIT			1
  #define MSR_IA32_MISC_ENABLE_TCC			(1ULL << MSR_IA32_MISC_ENABLE_TCC_BIT)
  #define MSR_IA32_MISC_ENABLE_EMON_BIT			7
  #define MSR_IA32_MISC_ENABLE_EMON			(1ULL << MSR_IA32_MISC_ENABLE_EMON_BIT)
  #define MSR_IA32_MISC_ENABLE_BTS_UNAVAIL_BIT		11
  #define MSR_IA32_MISC_ENABLE_BTS_UNAVAIL		(1ULL << MSR_IA32_MISC_ENABLE_BTS_UNAVAIL_BIT)
  #define MSR_IA32_MISC_ENABLE_PEBS_UNAVAIL_BIT		12
  #define MSR_IA32_MISC_ENABLE_PEBS_UNAVAIL		(1ULL << MSR_IA32_MISC_ENABLE_PEBS_UNAVAIL_BIT)
  #define MSR_IA32_MISC_ENABLE_ENHANCED_SPEEDSTEP_BIT	16
  #define MSR_IA32_MISC_ENABLE_ENHANCED_SPEEDSTEP	(1ULL << MSR_IA32_MISC_ENABLE_ENHANCED_SPEEDSTEP_BIT)
  #define MSR_IA32_MISC_ENABLE_MWAIT_BIT		18
  #define MSR_IA32_MISC_ENABLE_MWAIT			(1ULL << MSR_IA32_MISC_ENABLE_MWAIT_BIT)
  #define MSR_IA32_MISC_ENABLE_LIMIT_CPUID_BIT		22
  #define MSR_IA32_MISC_ENABLE_LIMIT_CPUID		(1ULL << MSR_IA32_MISC_ENABLE_LIMIT_CPUID_BIT)
  #define MSR_IA32_MISC_ENABLE_XTPR_DISABLE_BIT		23
  #define MSR_IA32_MISC_ENABLE_XTPR_DISABLE		(1ULL << MSR_IA32_MISC_ENABLE_XTPR_DISABLE_BIT)
  #define MSR_IA32_MISC_ENABLE_XD_DISABLE_BIT		34
  #define MSR_IA32_MISC_ENABLE_XD_DISABLE		(1ULL << MSR_IA32_MISC_ENABLE_XD_DISABLE_BIT)
  /* MISC_ENABLE bits: model-specific, meaning may vary from core to core */
  #define MSR_IA32_MISC_ENABLE_X87_COMPAT_BIT		2
  #define MSR_IA32_MISC_ENABLE_X87_COMPAT		(1ULL << MSR_IA32_MISC_ENABLE_X87_COMPAT_BIT)
  #define MSR_IA32_MISC_ENABLE_TM1_BIT			3
  #define MSR_IA32_MISC_ENABLE_TM1			(1ULL << MSR_IA32_MISC_ENABLE_TM1_BIT)
  #define MSR_IA32_MISC_ENABLE_SPLIT_LOCK_DISABLE_BIT	4
  #define MSR_IA32_MISC_ENABLE_SPLIT_LOCK_DISABLE	(1ULL << MSR_IA32_MISC_ENABLE_SPLIT_LOCK_DISABLE_BIT)
  #define MSR_IA32_MISC_ENABLE_L3CACHE_DISABLE_BIT	6
  #define MSR_IA32_MISC_ENABLE_L3CACHE_DISABLE		(1ULL << MSR_IA32_MISC_ENABLE_L3CACHE_DISABLE_BIT)
  #define MSR_IA32_MISC_ENABLE_SUPPRESS_LOCK_BIT	8
  #define MSR_IA32_MISC_ENABLE_SUPPRESS_LOCK		(1ULL << MSR_IA32_MISC_ENABLE_SUPPRESS_LOCK_BIT)
  #define MSR_IA32_MISC_ENABLE_PREFETCH_DISABLE_BIT	9
  #define MSR_IA32_MISC_ENABLE_PREFETCH_DISABLE		(1ULL << MSR_IA32_MISC_ENABLE_PREFETCH_DISABLE_BIT)
  #define MSR_IA32_MISC_ENABLE_FERR_BIT			10
  #define MSR_IA32_MISC_ENABLE_FERR			(1ULL << MSR_IA32_MISC_ENABLE_FERR_BIT)
  #define MSR_IA32_MISC_ENABLE_FERR_MULTIPLEX_BIT	10
  #define MSR_IA32_MISC_ENABLE_FERR_MULTIPLEX		(1ULL << MSR_IA32_MISC_ENABLE_FERR_MULTIPLEX_BIT)
  #define MSR_IA32_MISC_ENABLE_TM2_BIT			13
  #define MSR_IA32_MISC_ENABLE_TM2			(1ULL << MSR_IA32_MISC_ENABLE_TM2_BIT)
  #define MSR_IA32_MISC_ENABLE_ADJ_PREF_DISABLE_BIT	19
  #define MSR_IA32_MISC_ENABLE_ADJ_PREF_DISABLE		(1ULL << MSR_IA32_MISC_ENABLE_ADJ_PREF_DISABLE_BIT)
  #define MSR_IA32_MISC_ENABLE_SPEEDSTEP_LOCK_BIT	20
  #define MSR_IA32_MISC_ENABLE_SPEEDSTEP_LOCK		(1ULL << MSR_IA32_MISC_ENABLE_SPEEDSTEP_LOCK_BIT)
  #define MSR_IA32_MISC_ENABLE_L1D_CONTEXT_BIT		24
  #define MSR_IA32_MISC_ENABLE_L1D_CONTEXT		(1ULL << MSR_IA32_MISC_ENABLE_L1D_CONTEXT_BIT)
  #define MSR_IA32_MISC_ENABLE_DCU_PREF_DISABLE_BIT	37
  #define MSR_IA32_MISC_ENABLE_DCU_PREF_DISABLE		(1ULL << MSR_IA32_MISC_ENABLE_DCU_PREF_DISABLE_BIT)
  #define MSR_IA32_MISC_ENABLE_TURBO_DISABLE_BIT	38
  #define MSR_IA32_MISC_ENABLE_TURBO_DISABLE		(1ULL << MSR_IA32_MISC_ENABLE_TURBO_DISABLE_BIT)
  #define MSR_IA32_MISC_ENABLE_IP_PREF_DISABLE_BIT	39
  #define MSR_IA32_MISC_ENABLE_IP_PREF_DISABLE		(1ULL << MSR_IA32_MISC_ENABLE_IP_PREF_DISABLE_BIT)

#endif /* _ASM_X86_MSR_H_ */
