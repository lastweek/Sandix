/* Intel x86 cpu page infrastructure */
#ifndef _ASM_PAGE_H_
#define _ASM_PAGE_H_

#include <sandix/const.h>
#include <sandix/types.h>

#define PAGE_SHIFT	12
#define PAGE_SIZE	(_AC(1,UL) << PAGE_SHIFT)
#define PAGE_MASK	(~(PAGE_SIZE-1))

#endif /* _ASM_PAGE_H_ */
