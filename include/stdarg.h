/*
 *	2015/03/28 Created by Shan Yizhou.
 *	
 *	stdarg.h: Variable arguments list manipulation header.
 *	The header <stdarg.h> provides facilities for stepping
 *	through a list of function arguments of unknow number
 *	and types.
 *
 *	Suppose [last] is the last named parameter of a function
 *	pushed into stack by caller.
 */

#ifndef STDC_STDARG_H
#define STDC_STDARG_H

/* Stack alignment: 4 bytes.*/
#define STACK_ALIGN	4

typedef char * va_list

#define va_start(ap, last)	ap = (va_list)&last + STACK_ALIGN
#define va_copy(dest, src)	dest = src
#define va_end(ap)			ap = NULL
#define va_arg(ap, type)	*((type *)ap); ap += STACK_ALIGN

#endif	/* STDC_STDARG_H */
