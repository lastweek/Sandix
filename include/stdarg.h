/* FIXME double float */
#ifndef STDARG_H
#define STDARG_H

/* Stack alignment: 4 bytes.*/
#define STACK_ALIGN	4

typedef char * va_list;

#define va_start(ap, last)	ap = (va_list)&last + STACK_ALIGN

#define va_copy(dest, src)	dest = src

#define va_end(ap)			ap = NULL

#define va_arg(ap, type)		\
	({							\
		type __re;				\
		__re = *((type *)ap);	\
		ap += STACK_ALIGN;		\
		__re;					\
	})

#endif /* STDARG_H */
