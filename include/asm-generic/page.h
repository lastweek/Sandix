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

/*
 * Generic page implementation, for NOMMU architectures
 */

#ifndef _ASM_GENERIC_PAGE_H_
#define _ASM_GENERIC_PAGE_H_

#ifdef CONFIG_MMU
# error "Please provide a real <asm/page.h>"
#endif

/*
 * PAGE_SHIFT determines the page size
 */

#define PAGE_SHIFT	12

#ifdef __ASSEMBLY__
# define PAGE_SIZE	(1 << PAGE_SHIFT)
#else
# define PAGE_SIZE	(1UL << PAGE_SHIFT)
#endif

#define PAGE_MASK	(~(PAGE_SIZE-1))

#ifndef __ASSEMBLY__

/*
 * These are used to make use of C type-checking
 */

typedef struct {
	unsigned long pte;
} pte_t;

typedef struct {
	unsigned long pmd[16];
} pmd_t;

typedef struct {
	unsigned long pgd;
} pgd_t;

typedef struct {
	unsigned long pgprot;
} pgprot_t;

typedef struct page *pgtable_t;

#define pte_val(x)	((x).pte)
#define pmd_val(x)	((&x)->pmd[0])
#define pdg_val(x)	((x).pgd)
#define pgprot_val(x)	((x).pgprot)

#define __pte(x)	((pte_t) { (x) } )
#define __pmd(x)	((pmd_t) { (x) } )
#define __pgd(x)	((pgd_t) { (x) } )
#define __pgprot(x)	((pgprot_t) { (x) } )

/*
 * no mmu, no page shift
 */

#define __va(x) ((void *)((unsigned long) (x)))
#define __pa(x) ((unsigned long) (x))

#define virt_to_pfn(kaddr)	(__pa(kaddr) >> PAGE_SHIFT)
#define pfn_to_virt(pfn)	__va((pfn) << PAGE_SHIFT)

#endif /* __ASSEMBLY__ */

#endif /* _ASM_GENERIC_PAGE_H_ */
