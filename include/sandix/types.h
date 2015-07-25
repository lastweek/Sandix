#ifndef _SANDIX_TYPES_H_
#define _SANDIX_TYPES_H_

#ifndef __ASSEMBLY__

#ifndef	NULL
#define	NULL	((void *)0)
#endif

#define	bool int
enum {
	false,
	true
};

typedef unsigned long long	u64;
typedef unsigned int		u32;
typedef unsigned short		u16;
typedef unsigned char		u8;
typedef unsigned long long	__u64;
typedef unsigned int		__u32;
typedef unsigned short		__u16;
typedef unsigned char		__u8;

typedef u32 size_t;
typedef u32 pit_t;
typedef u32 phys_addr_t;

/*
 * Use typedef struct to avoid some program mistake.
 * If typedef int atomic_t directly, then compiler
 * will not complain about your incorrect usage.
 */
typedef struct {
	int counter;
} atomic_t;

#endif /* __ASSEMBLY__ */

#endif /* _SANDIX_TYPES_H_ */
