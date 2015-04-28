#include <stdio.h>

static inline unsigned long long asm_rdtscp(void)
{
	unsigned hi, lo;
	asm volatile ("rdtscp" : "=a"(lo), "=d"(hi)::"ecx");
    return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}

int main()
{
	unsigned long long tsc = asm_rdtscp();

	printf("%llu\n", tsc);
}
