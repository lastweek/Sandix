/* Arch-Independent, system manageable page structure */
#ifndef _SANDIX_PAGE_H_
#define _SANDIX_PAGE_H_

#include <sandix/types.h>
#include <sandix/atomic.h>
#include <asm/page.h> /* Arch-Dependent page infrastructure */

enum SANDIX_PAGE_FLAGS {
	
};

/*
 *	Each physical page in the system has a struct page
 *	associated with it to keep track of whatever it is
 *	we are using the page for at the moment.
 */
struct page {
	unsigned long flag;
	atomic_t count;
	void *virtual;
};

#endif /* _SANDIX_PAGE_H_*/
