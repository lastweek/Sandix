/* Intel x86 2-level paging infrastructure */
#ifndef _ASM_PAGE_H_
#define _ASM_PAGE_H_

#include <sandix/const.h>
#include <sandix/types.h>

#define PAGE_SHIFT	12
#define PAGE_SIZE	(_AC(1,UL) << PAGE_SHIFT)
#define PAGE_MASK	(~(PAGE_SIZE-1))


/*
 * Physical / Virtual address mask.
 * Useless now!
 */
#define PHYS_MASK_SHIFT		32
#define VIRT_MASK_SHIFT		32

/*
 * The possible maximum page frame number
 */
#define MAX_ARCH_PFN	(1ULL << (32 - PAGE_SHIFT))

/*
 * Kernel base virtual address 0xC0000000 limits the
 * memory space kernel can use to one gigabyte.
 */
#define __PAGE_OFFSET		0xC0000000
#define PAGE_OFFSET		_AT(unsigned long, __PAGE_OFFSET)

#define __phys_addr(x)		((x) - PAGE_OFFSET)
#define __pa(x)			__phys_addr(_AT(unsigned long, x))
#define __va(x)			(_AT(unsigned long, x) + PAGE_OFFSET)

/*
 * Thread size in kernel can be one page or two pages.
 * The default is two pages each thread in Sandix.
 */
#define THREAD_PAGE_NR		1
#define THREAD_SIZE		(PAGE_SIZE << THREAD_PAGE_NR)

#endif /* _ASM_PAGE_H_ */
