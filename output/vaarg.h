/*
*	Variable length arguments operation header.
*	2015/03/28 Created by Shan Yizhou.
*
*	Notice: 
*	1.
*		Different Architecture or OS use different Calling Conventions.
*		This code will only conform to SystemV 32bit&64bit conventions.
*	2.
*		Strings in C is NULL-Terminated by default. Which means if you
*		write printf("Hello") in C code, the actual string in assembly
*		is "Hello\0", which also means it will take 6 bytes in file or mem.
*/

#ifndef VAARG_HEADER
#define VAARG_HEADER

#define	STACK_ALIGN_64	8
#define STACK_ALIGN_32	4

char *var_addr;
#define var_end()		var_addr = NULL

/*
 *	It's weird. I do not know why gcc use -200(%rbp)
 *	to get the first argument. Obviously gcc has already
 *	push 1st argument besides 2st argument. See assembly souce.
 *
 */
#ifndef	x86_64_SANDIX
#define var_init(addr)	var_addr = (char *)addr + 32
#define var_arg(arg, type)	\
	do{\
		arg = *((type *)var_addr);\
		var_addr += STACK_ALIGN_64;\
	}while(0)
#else
#define var_init(addr)	var_addr = (char *)addr + STACK_ALIGN_32
#define var_arg(arg, type)	\
	do{\
		arg = *((type *)var_addr);\
		var_addr += STACK_ALIGN_32;\
	}while(0)
#endif	/* x86_64_SANDIX */

#endif	/* VAARG_HEADER */
