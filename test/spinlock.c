#include <stdio.h>

typedef struct spinlock {
	unsigned int lock;
} spinlock_t;

static inline void spin_lock(spinlock_t *lock)
{
	asm volatile (
		"1:			\n\t"
		"	lock; decb %0	\n\t"
		"	jns 3f		\n\t"
		"2:			\n\t"
		"	cmpb $0, %0	\n\t"
		"	rep; nop	\n\t"
		"	jle 2b		\n\t"
		"	jmp 1b		\n\t"
		"3:			\n\t"
		: "+m" (lock->lock) : : "memory"
	);
}

int main(void)
{
	spinlock_t slock = { 0 };
	
	slock.lock = 9;
	slock.lock = 0;

	asm volatile (
		"movl %1, %%eax\n\t"
		"decb %%al\n\t"
		: "+m" (slock.lock) : : "memory"
	);

	printf(" %x \n", slock.lock);

	//spin_lock(&slock);

	return 0;
}
