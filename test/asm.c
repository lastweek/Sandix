#include <stdio.h>

#define LOCK "lock; "

int main(){
	int x=1,y=2,z=3;

	asm volatile (
		LOCK "addl %1, %0"
		:"=m"(y)
		:"ir"(x)
	);
	
	asm volatile (
		"addl %1, %0"
		:"+r"(y)
		:"ir"(x)
	);
}
