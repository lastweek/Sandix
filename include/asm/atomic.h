#ifndef _SAM_ATOMIC_H_
#define _SAM_ATOMIC_H_

/*
 *	Machine-dependent atomic operations.
 *	IA-32
 */

#include <sandix/types.h>

typedef struct {
	int counter;
} atomic_t;


#endif /* _SAM_ATOMIC_H_ */
