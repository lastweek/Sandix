#include <stdio.h>

#define LOCK "lock; "

static inline void add_1(int *p, int c)
{
	asm volatile(
		"addl %1, %0"
		: "+m" (*p)
		: "ir" (c)
	);
}

static inline void add_2(int *p, int c)
{
	asm volatile(
		"addl %1, %0"
		: "=m" (*p)
		: "ir" (c), "m"(p)
	);
}

static inline void _atomic_or(int *p, int mask)
{
	asm volatile ("lock; orl %1, %0"
		: "+r" (*p)
		: "r" (mask)
	);
}
static inline void atomic_or(int *p, int mask)
{
	asm volatile ("lock; orl %1, %0"
		: "+m" (*p)
		: "r" (mask)
	);
}

int main(){
	int x=1,y=2,z=3;
	
	atomic_or(&z, 0xf);
	_atomic_or(&z, 0xf);

	add_1(&x, 100);

	add_2(&x, 200);

	z = y+x;
	
	return z;
}
