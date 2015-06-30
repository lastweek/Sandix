/* Intel x86 2-level paging infrastructure */
#ifndef _ASM_PAGE_H_
#define _ASM_PAGE_H_

#include <sandix/const.h>
#include <sandix/types.h>

#define PAGE_SHIFT	12
#define PAGE_SIZE	(_AC(1,UL) << PAGE_SHIFT)
#define PAGE_MASK	(~(PAGE_SIZE-1))

/*
 * Kernel base virtual address 0xC0000000 limits the
 * memory space kernel can use to one gigabyte.
 */
#define __PAGE_OFFSET	0xC0000000
#define PAGE_OFFSET 	((unsigned long)__PAGE_OFFSET)

#define __phys_addr(x)	((x) - PAGE_OFFSET)
#define __pa(x)			__phys_addr((unsigned long)x)
#define __va(x)			((unsigned long)(x) + PAGE_OFFSET)

/*
 * Thread size in kernel can be one page or two pages.
 * The default is two pages each thread.
 */
#define THREAD_SIZE_ORDER	1
#define THREAD_SIZE 		(PAGE_SIZE << THREAD_SIZE_PAGE_NR)

#endif /* _ASM_PAGE_H_ */
