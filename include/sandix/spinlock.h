#ifndef _SANDIX_SPINLOCK_H_
#define _SANDIX_SPINLOCK_H_

#include <asm/spinlock.h>

typedef struct spinlock {
	struct arch_spinlock alock;
} spinlock_t;

static inline void spin_lock(spinlock_t *lock)
{
	arch_spin_lock(&lock->alock);
}

static inline int spin_trylock(spinlock_t *lock)
{
	return arch_spin_trylock(&lock->alock);
}

static inline void spin_unlock(spinlock_t *lock)
{
	arch_spin_unlock(&lock->alock);
}

#endif /* _SANDIX_SPINLOCK_H_ */
