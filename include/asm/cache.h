#ifndef _ASM_CACHE_H_
#define _ASM_CACHE_H_

#define L1_CACHE_LINE_SIZE	64

/*
 *	Normally, if one byte in a cache line is diry, then the entire
 *	cache line will need to be flushed back. Put the read most, which also,
 *	write rarely data together, then a lot of useless cache line write
 *	events can be avoided.
 */
#define __read_mostly	__attribute__((section(".data..read_mostly")))

#endif /* _ASM_CACHE_H_ */
