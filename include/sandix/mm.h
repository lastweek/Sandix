#ifndef _SANDIX_MM_H_
#define _SANDIX_MM_H_

#include <sandix/types.h>
#include <sandix/atomic.h>
#include <sandix/page.h>

struct mm_struct {
	int a;
};

enum ZONE_TYPE {
	ZONE_NORMAL,
	ZONE_HIGHMEM,
	ZONE_DMA
};

struct zone {
	int zone_type;
};

extern unsigned long max_low_pfn;
extern unsigned long min_low_pfn;
extern unsigned long max_pfn;

void setup_memory_map(void);
void init_memory(void);

#endif /* _SANDIX_MM_H_ */
