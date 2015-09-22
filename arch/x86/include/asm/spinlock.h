#ifndef _ASM_SPINLOCK_H_
#define _ASM_SPINLOCK_H_

#ifndef _SANDIX_SPINLOCK_H_
# error "Please do not include this file directly, include <sandix/spinlock.h>"
#endif

typedef struct arch_spinlock {

} arch_spinlock_t;

static inline void arch_spin_lock(arch_spinlock_t *lock)
{

}

static inline int arch_spin_trylock(arch_spinlock_t *lock)
{
	
}

static inline void arch_spin_unlock(arch_spinlock_t *lock)
{

}

#endif /* _ASM_SPINLOCK_H_ */
