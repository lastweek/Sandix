#ifndef _ASM_ASM_H_
#define _ASM_ASM_H_

#define LOCK_PREFIX "lock; "

#define nop()	native_nop()
#define mb()	native_mb()
#define rmb()	native_rmb()
#define wmb()	native_wmb()

static inline void native_nop(void)
{
	asm volatile ("nop");
}

static inline void native_mb(void)
{
	asm volatile ("mfence");
}

static inline void native_rmb(void)
{
	asm volatile ("lfence");
}

static inline void native_wmb(void)
{
	asm volatile ("wfence");
}

#endif /* _ASM_ASM_H_ */
