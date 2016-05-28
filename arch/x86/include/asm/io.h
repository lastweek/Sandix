/*
 *	Copyright (C) 2015-2016 Yizhou Shan <shanyizhou@ict.ac.cn>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License along
 *	with this program; if not, write to the Free Software Foundation, Inc.,
 *	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _ASM_X86_IO_H_
#define _ASM_X86_IO_H_

#include <asm/page.h>
#include <sandix/types.h>
#include <sandix/compiler.h>

#define BUILD_MMIO_READ(name, size, type, reg, barrier)		\
static inline type name(const volatile void *addr)		\
{								\
	type ret;						\
	asm volatile (						\
		"mov" size " %1,%0"				\
		: reg (ret)					\
		: "m" (*(volatile type *)addr)			\
		barrier						\
	);							\
	return ret;						\
}

#define BUILD_MMIO_WRITE(name, size, type, reg, barrier)	\
static inline void name(type val, volatile void *addr)		\
{								\
	asm volatile (						\
		"mov" size " %0,%1"				\
		:						\
		: reg (val), "m" (*(volatile type *)addr)	\
		barrier						\
	);							\
}

BUILD_MMIO_READ(readb, "b", unsigned char,  "=q", :"memory")
BUILD_MMIO_READ(readw, "w", unsigned short, "=r", :"memory")
BUILD_MMIO_READ(readl, "l", unsigned int,   "=r", :"memory")

BUILD_MMIO_READ(__readb, "b", unsigned char,  "=q", )
BUILD_MMIO_READ(__readw, "w", unsigned short, "=r", )
BUILD_MMIO_READ(__readl, "l", unsigned int,   "=r", )

BUILD_MMIO_WRITE(writeb, "b", unsigned char,  "q", :"memory")
BUILD_MMIO_WRITE(writew, "w", unsigned short, "r", :"memory")
BUILD_MMIO_WRITE(writel, "l", unsigned int,   "r", :"memory")

BUILD_MMIO_WRITE(__writeb, "b", unsigned char,  "q", )
BUILD_MMIO_WRITE(__writew, "w", unsigned short, "r", )
BUILD_MMIO_WRITE(__writel, "l", unsigned int,   "r", )

#ifdef CONFIG_X86_64
BUILD_MMIO_READ(readq, "q", unsigned long, "=r", :"memory")
BUILD_MMIO_WRITE(writeq, "q", unsigned long, "r", :"memory")
#endif

/**
 *	virt_to_phys	-	map virtual addresses to physical
 *	@address: address to remap
 *
 *	The returned physical address is the physical (CPU) mapping for
 *	the memory address given. It is only valid to use this function on
 *	addresses directly mapped or allocated via kmalloc.
 *
 *	This function does not give bus mappings for DMA transfers. In
 *	almost all conceivable cases a device driver should not be using
 *	this function
 */
static inline phys_addr_t virt_to_phys(volatile void *address)
{
	return __pa(address);
}

/**
 *	phys_to_virt	-	map physical address to virtual
 *	@address: address to remap
 *
 *	The returned virtual address is a current CPU mapping for
 *	the memory address given. It is only valid to use this function on
 *	addresses that have a kernel mapping
 *
 *	This function does not handle bus mappings for DMA transfers. In
 *	almost all conceivable cases a device driver should not be using
 *	this function
 */
static inline void *phys_to_virt(phys_addr_t address)
{
	return __va(address);
}

static inline void native_io_delay(void)
{
	asm volatile ("outb %al, $0x80");
}

#define BUILD_IO_OPS(bwl, bw, type)					\
static inline void out##bwl(unsigned type value, int port)		\
{									\
	asm volatile("out" #bwl " %" #bw "0, %w1"			\
		     : : "a"(value), "Nd"(port));			\
}									\
									\
static inline unsigned type in##bwl(int port)				\
{									\
	unsigned type value;						\
	asm volatile("in" #bwl " %w1, %" #bw "0"			\
		     : "=a"(value) : "Nd"(port));			\
	return value;							\
}									\
									\
static inline void out##bwl##_p(unsigned type value, int port)		\
{									\
	out##bwl(value, port);						\
	native_io_delay();						\
}									\
									\
static inline unsigned type in##bwl##_p(int port)			\
{									\
	unsigned type value = in##bwl(port);				\
	native_io_delay();						\
	return value;							\
}									\
									\
static inline void outs##bwl(int port, const void *addr,		\
			     unsigned long count)			\
{									\
	asm volatile("rep; outs" #bwl					\
		     : "+S"(addr), "+c"(count) : "d"(port));		\
}									\
									\
static inline void ins##bwl(int port, void *addr, unsigned long count)	\
{									\
	asm volatile("rep; ins" #bwl					\
		     : "+D"(addr), "+c"(count) : "d"(port));		\
}

BUILD_IO_OPS(b, b, char)
BUILD_IO_OPS(w, w, short)
BUILD_IO_OPS(l, , int)

#endif /* _ASM_X86_IO_H_*/
