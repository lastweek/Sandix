#include <stdio.h>

#define LOCK "lock; "

#define LOCK_PREFIX_HERE \
		".pushsection .smp_locks,\"a\"\n"	\
		".balign 4\n"				\
		".long 671f - .\n" /* offset */		\
		".popsection\n"				\
		"671:"

#define LOCK_PREFIX LOCK_PREFIX_HERE "\n\tlock; "

int main(){
	int x=1,y=2,z=3;

	asm volatile (
		LOCK_PREFIX "addl %1, %0"
		:"=m"(y)
		:"ir"(x)
	);
	
	asm volatile (
		"addl %1, %0"
		:"+r"(y)
		:"ir"(x)
	);
}
