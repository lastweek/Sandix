/*
 *	2015/04/16 Created by Shan Yizhou.
 *
 *	types.h: Basic type definitions for x86 cpus.
 *
 */

#ifndef SANDIX_TYPES_H
#define SANDIX_TYPES_H

#ifndef NULL
#define NULL	((void *)0)
#endif

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
#endif
