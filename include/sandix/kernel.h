#ifndef _SANDIX_KERNEL_H_
#define _SANDIX_KERNEL_H_

#include <sandix/types.h>

#define offsetof(TYPE, MEMBER) \
	(((unsigned int)) &((TYPE *)0)->MEMBER)

/**
 * container_of - find which structure this @ptr located
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member)						\
	({														\
		typeof( ((type *)0)->member ) *__ptr = (ptr);		\
		(type *)( (char *)__ptr - offsetof(type,member) );	\
	})

#endif /* _SANDIX_KERNEL_H_ */
