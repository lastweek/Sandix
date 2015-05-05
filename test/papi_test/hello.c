#include <stdio.h>
/*
CPUID.OAH Leaf:
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

unsigned int eax_architectual_perf_version;
unsigned int eax_nr_of_perf_counter_per_cpu;
unsigned int eax_bit_width_of_perf_counter;
unsigned int eax_len_of_ebx_to_enumerate;

unsigned int ebx_predefined_event_mask;

unsigned int edx_nr_of_fixed_func_perf_counter;
unsigned int edx_bit_width_of_fixed_func_perf_counter;

void cpuid(unsigned int *a, unsigned int *b, unsigned int *c, unsigned int *d)
{
	unsigned int op = *a;
	asm("cpuid;"
		: "=a" (*a), "=b" (*b), "=c" (*c), "=d" (*d)
		: "a" (op) );
}

void get_native_perf_info(void)
{
	unsigned int eax, ebx, ecx, edx;
	eax = 0x0A;
	cpuid(&eax, &ebx, &ecx, &edx);
	printf("eax = %X\nebx = %X\necx = %X\nedx = %X\n", a,b,c,d);

}

int main()
{
	unsigned int eax, ebx, ecx, edx;
	printf("eax = %X\nebx = %X\necx = %X\nedx = %X\n", a,b,c,d);
}
