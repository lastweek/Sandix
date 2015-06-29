#ifndef _SANDIX_MM_H
#define _SANDIX_MM_H

/*
 *	Page Descriptor.
 *	Each physical page in the system has a struct page associated with it
 *	to keep track of whatever it is we are using the page for at the moment.
 */
struct page {
	unsigned long flag;

	void *virtual;
};


struct mm_struct {
	
};

#endif /* _SANDIX_MM_H */
