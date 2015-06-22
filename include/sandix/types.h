#ifndef _SANDIX_TYPES_H_
#define _SANDIX_TYPES_H_

#ifndef NULL
#define NULL	((void *)0)
#endif

#undef bool
#define bool	int

#undef
#define true	1

#undef
#define false	0

typedef unsigned	long long	__u64;
typedef unsigned 	long long	u64;
typedef unsigned	int			__u32;
typedef unsigned 	int			u32;
typedef unsigned	short		__u16;
typedef unsigned 	short		u16;
typedef unsigned	char		__u8;
typedef unsigned 	char		u8;

typedef signed		long long	__s64;
typedef signed		long long	s64;
typedef signed   	int			__s32;
typedef signed		int			s32;
typedef signed   	short		__s16;
typedef signed		short		s16;
typedef signed   	char		__s8;
typedef signed		char		s8;

typedef unsigned int	size_t;
typedef unsigned int 	addr_t;
typedef unsigned int	pit_t;

#endif /* _SANDIX_TYPES_H_ */
