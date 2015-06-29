#ifndef _SANDIX_TYPES_H_
#define _SANDIX_TYPES_H_

#ifndef	NULL
#define	NULL	((void *)0)
#endif

#undef	bool
#define	bool	int

#undef	true
#define	true	1

#undef	false
#define	false	0

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

#endif /* _SANDIX_TYPES_H_ */
