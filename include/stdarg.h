/* 32-bit usable */
#ifndef _STDARG_H_
#define _STDARG_H_

typedef char *va_list;

#define __STACK_ALIGN	sizeof(long)

#define __va_rounded_size(TYPE)	\
	(((sizeof(TYPE) + __STACK_ALIGN - 1) / __STACK_ALIGN) * __STACK_ALIGN)

#define va_start(AP, LASTARG)	\
	AP = ((va_list)&(LASTARG) + __va_rounded_size(LASTARG))

#define va_copy(DEST, SRC)	\
	DEST = SRC

#define va_end(AP)	\
	AP = NULL

#define va_arg(AP, TYPE)				\
	({									\
		TYPE __ret;						\
		__ret = *((TYPE *)(AP));		\
		AP += __va_rounded_size(TYPE);	\
		__ret;							\
	})

#endif /* _STDARG_H_ */
