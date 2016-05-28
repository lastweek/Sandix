/*
 *	Copyright (C) 2016 Yizhou Shan <shan13@purdue.edu>
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

#ifndef _ASM_GENERIC_EARLY_IOREMAP_H_
#define _ASM_GENERIC_EARLY_IOREMAP_H_

#include <sandix/types.h>

/*
 * early_ioremap() and early_iounmap() are for temporary early boot-time
 * mappings, before the real ioremap() is functional.
 */
void __iomem *early_ioremap(resource_size_t phys_addr, unsigned long size);
void *early_memremap(resource_size_t phys_addr, unsigned long size);
void *early_memremap_ro(resource_size_t phys_addr, unsigned long size);
void early_iounmap(void __iomem *addr, unsigned long size);
void early_memunmap(void *addr, unsigned long size);

/*
 * Weak function called by early_ioremap_reset(). It does nothing, but
 * architectures may provide their own version to do any needed cleanups.
 */
void early_ioremap_shutdown(void);

#if defined(CONFIG_GENERIC_EARLY_IOREMAP) && defined(CONFIG_MMU)
/* Arch-specific initialization */
void early_ioremap_init(void);

/* Generic initialization called by architecture code */
void early_ioremap_setup(void);

/*
 * Called as last step in paging_init() so library can act
 * accordingly for subsequent map/unmap requests.
 */
void early_ioremap_reset(void);

/*
 * Early copy from unmapped memory to kernel mapped memory.
 */
void copy_from_early_mem(void *dest, phys_addr_t src, unsigned long size);

#else
static inline void early_ioremap_init(void) { }
static inline void early_ioremap_setup(void) { }
static inline void early_ioremap_reset(void) { }
#endif

#endif /* _ASM_GENERIC_EARLY_IOREMAP_H_ */
