#ifndef _SANDIX_TYPES_H_
#define _SANDIX_TYPES_H_

#ifndef NULL
#define NULL	((void *)0)
#endif

#ifndef bool
#define bool	int
#endif

#ifndef true
#define true	1
#endif

#ifndef false
#define false	0
#endif

typedef unsigned long long	__u64;
typedef unsigned int		__u32;
typedef unsigned short		__u16;
typedef unsigned char		__u8;
typedef signed long long	__s64;
typedef signed   int		__s32;
typedef signed   short		__s16;
typedef signed   char		__s8;
typedef unsigned long long	u64;
typedef unsigned int		u32;
typedef unsigned short		u16;
typedef unsigned char		u8;
typedef signed long long	s64;
typedef signed   int		s32;
typedef signed   short		s16;
typedef signed   char		s8;

typedef unsigned int	size_t;
typedef unsigned int 	addr_t;

/* process id */
typedef unsigned int	pit_t;

struct list_head {
	struct list_head *next;
	struct list_head *prev;
};

#endif /* _SANDIX_TYPES_H_ */
