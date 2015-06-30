#ifndef _SAM_ATOMIC_H_
#define _SAM_ATOMIC_H_

/*
 *	Machine-dependent atomic operations.
 *	IA-32 Architecture.
 */

#include <sandix/types.h>
#include <asm/asm.h>

typedef struct {
	int counter;
} atomic_t;

static inline int
atomic_read(const atomic_t *v)
{
	return v->counter;
}

static inline void
atomic_set(atomic_t *v, int i)
{
	v->counter = i;
}

static inline void
atomic_add(int i, atomic_t *v)
{
	asm volatile (
		LOCK_PREFIX "addl %1, %0"
		: "=m"(v->counter)
		: "ir"(i)
	);
}

#endif /* _SAM_ATOMIC_H_ */
