#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
EAX Bits 07 - 00: Version ID of architectural performance monitoring
	Bits 15 - 08: Number of general-purpose performance monitoring counter per logical processor
	Bits 23 - 16: Bit width of general-purpose, performance monitoring counter
	Bits 31 - 24: Length of EBX bit vector to enumerate architectural performance monitoring events

EBX Bit 00: Core cycle event not available if 1
	Bit 01: Instruction retired event not available if 1
	Bit 02: Reference cycles event not available if 1
	Bit 03: Last-level cache reference event not available if 1
	Bit 04: Last-level cache misses event not available if 1
	Bit 05: Branch instruction retired event not available if 1
	Bit 06: Branch mispredict retired event not available if 1
	Bits 31- 07: Reserved = 0

ECX Reserved = 0

EDX Bits 04 - 00: Number of fixed-function performance counters (if Version ID > 1)
	Bits 12 - 05: Bit width of fixed-function performance counters (if Version ID > 1)
	Reserved = 0
*/

unsigned int eax_arch_perf_version;
unsigned int eax_nr_of_perf_counter_per_cpu;
unsigned int eax_bit_width_of_perf_counter;
unsigned int eax_len_of_ebx_to_enumerate;
unsigned int ebx_predefined_event_mask;
unsigned int edx_nr_of_fixed_func_perf_counter;
unsigned int edx_bit_width_of_fixed_func_perf_counter;

unsigned long long CPU_BASE_FREQUENCY;
char CPU_BRAND[48];


void
err(char *fmt)
{
	printf("%s");
	exit(-1);
}


/* x86_64 Only */
void
cpuid(unsigned int *eax, unsigned int *ebx, unsigned int *ecx, unsigned int *edx)
{
	unsigned int op = *eax;
	asm volatile(
		"cpuid"
		: "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
		: "a"(op)
	);
}


/* Time Stamp Counter Addr: 0x10 in MSR */
unsigned long long
rdtsc(void)
{
	unsigned int edx, eax;
	unsigned long long retval = 0;

	asm volatile(
		"rdtsc"
		:"=a"(eax), "=d"(edx)
	);

	retval = ((unsigned long long)(edx) << 32) | (unsigned long long)(eax);
	return retval;
}


/* Privilege 0 or Real-Mode */
unsigned long long
rdmsr(unsigned int addr)
{
	unsigned int edx, eax;
	unsigned long long retval = 0;
	
	asm volatile(
		"rdtsc"
		:"=a"(eax), "=d"(edx)
		:"c"(addr)
	);

	retval =((unsigned long long)(edx) << 32) | (unsigned long long)(eax);
	return retval;
}


/* See Intel Developer-Manual 2 [CPUID] */
void
cpu_brand_frequency(void)
{
	char *s;
	unsigned int i;
	unsigned int eax, ebx, ecx, edx;

	eax = 0x80000000;
	cpuid(&eax, &ebx, &ecx, &edx);

	if (eax < 0x80000004U)
		err("CPUID, Extended function Not supported.\n");
	
	s = CPU_BRAND;
	for (i = 0; i < 3; i++) {
		eax = 0x80000002U + i;
		cpuid(&eax, &ebx, &ecx, &edx);
		memcpy(s, &eax, 4); s += 4;
		memcpy(s, &ebx, 4); s += 4;
		memcpy(s, &ecx, 4); s += 4;
		memcpy(s, &edx, 4); s += 4;
	}

	CPU_BASE_FREQUENCY = 2000000000;
	//CPU_FREQUENCY = 1995381060;
}


void
cpu_basic_info(void)
{
	unsigned int eax, ebx, ecx, edx;
	
	eax = 0x01;
	cpuid(&eax, &ebx, &ecx, &edx);
	
}


/* Get Host CPU Performance Monitoring Information */
void
cpu_perf_info(void)
{
	unsigned int eax, ebx, ecx, edx;
	
	eax = 0x0A;
	cpuid(&eax, &ebx, &ecx, &edx);
	
	eax_arch_perf_version			= eax & 0xFFU;
	eax_nr_of_perf_counter_per_cpu	= (eax & 0xFF00U) >> 8;
	eax_bit_width_of_perf_counter	= (eax & 0xFF0000U) >> 16;
	eax_len_of_ebx_to_enumerate 	= (eax & 0xFF000000U) >> 24;

	ebx_predefined_event_mask		= ebx;

	edx_nr_of_fixed_func_perf_counter			= edx & 0x1FU;
	edx_bit_width_of_fixed_func_perf_counter	= (edx & 0x1FE0U) >> 5;
}

/* Get Host CPU General Information */
void
cpu_general_info(void)
{
	unsigned int eax, ebx, ecx, edx;
	
	/* Get Basic CPU info */
	cpu_basic_info();

	/* Get CPU Brand&Frequency */
	cpu_brand_frequency();
	
	/* Get CPU PM info */
	cpu_perf_info();
}

void
cpu_info_print(void)
{
	printf("\n*****************HOST CPU INFORMATION*****************\n");
	printf("%s\n", CPU_BRAND);
	
	printf("Architectual Performance Monitoring Version ID: %u\n", eax_arch_perf_version);
	printf("Number of general-purpose perf counter per cpu: %u\n", eax_nr_of_perf_counter_per_cpu);
	printf("Bit width of general-purpose, perf counter reg: %u\n", eax_bit_width_of_perf_counter);
	printf("Length of [EBX] bit vector to enumerate events: %u\n", eax_len_of_ebx_to_enumerate);
	printf("EBX events not avaliable if 1: %x\n", ebx_predefined_event_mask);
	
	if (eax_arch_perf_version > 1) {
		printf("Number of fixed-func perf counters:    %u\n", edx_nr_of_fixed_func_perf_counter);
		printf("Bit width of fixed-func perf counters: %u\n", edx_bit_width_of_fixed_func_perf_counter);
	}
	
	printf("******************************************************\n\n");
}


int main()
{
	double t1, t2;

	cpu_general_info();
	cpu_info_print();

	t1 = (double)rdtsc();
	sleep(2);
	t2 = (double)rdtsc();
	
	t1 = t2 - t1;
	t1 = t1/CPU_BASE_FREQUENCY * 1000;
	printf("%lf\n", t1);
}



