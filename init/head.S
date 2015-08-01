/*
 *	init/head.S - Kernel Entry Point
 *
 *	Copyright (C) 2015 Yizhou Shan <shanyizhou@ict.ac.cn>
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

#include <sandix/linkage.h>
#include <asm/bootparam.h>
#include <asm/page.h>
#include <asm/segment.h>

/*
 * We have already set the virtual address base of
 * kernel to (0x10000+__PAGE_OFFSET) in linker scripts.
 * So here the physical address is equals to sympol's
 * virtual address minus __PAGE_OFFSET.
 */
#define T__PAGE_OFFSET	0x0
#define pa(X)	((X) - T__PAGE_OFFSET)

#define CR0_PG		0x80000000
#define CR0_MP  	0x00000002
#define CR0_PE		0x00000001
#define CR0_STATE	(CR0_PG|CR0_MP|CR0_PE)

	.code32
	.section .text

/*
 * 32-bit kernel entrypoint. Only used by the boot CPU(BP).
 * startup_32 initialize page directory, page table and
 * interrupt descriptor table(idt), which have exact only
 * one copy in memory respectively. Caveat: paging and
 * lgdt and lidt must be done on each CPU.
 */
ENTRY(startup_32)

/*
 * DEBUG
 * Show '@' in left-upper corner.
 */
	movl $0xb8000, %ebx
	movb $0x40, (%ebx)
/*
 * Reload segment registers.
 * We have done this before, but we do it again here
 * just for safey because we will use movsl below.
 */
 	xorl %eax, %eax
	movl $(__BOOT_DS), %eax
	movl %eax, %ds
	movl %eax, %ss
	movl %eax, %es
	movl %eax, %fs
	movl %eax, %gs

/*
 * Clear BSS to avoid abnormal things.
 */
 	cld
	xorl %eax, %eax
	movl $pa(__bss_start__), %edi
	movl $pa(__bss_end__), %ecx
	subl %edi, %ecx
	shrl $2, %ecx
	rep ; stosl

/*
 * Move boot_param to a safe place.
 * Reg %esi still points to the struct boot_param in real mode.
 * The size of struct boot_param is one page size, 4096 bytes.
 */
	movl $pa(boot_params), %edi
	movl $(BOOT_PARAMS_SIZE), %ecx
	shrl $2, %ecx
	rep ; movsl

/*
 * Initialize early_page_directory and early_page_table.
 * Only one page table is present. And the logical address
 * is equal to the physical address after mapping!
 */
	movl $pa(early_page_table), %edi
	addl $7, %edi			# PRESENT R/W U/S
 	movl $pa(early_page_dir), %eax
	movl %edi, (%eax)
	
	movl $pa(early_page_table), %edi
	movl $7, %eax			# PRESENT R/W U/S
	movl $1024, %ecx
1:
	stosl
	addl $0x1000, %eax
	loop 1b

/*
 * Setup idt entries. We don't load idt here - that
 * need to be done on each cpu. Interrupts are enabled
 * elsewhere, when we sure everything is ok!!
 */
	movl $pa(idt_table), %edi
	movl $pa(ignore_int), %eax
	movl $IDT_ENTRIES, %ecx
1:
	movl %eax, (%edi)
	movl %eax, 4(%edi)
	movl $(0x8E000000+__KERNEL_CS), 2(%edi) # intr gate, dpl=0
	addl $8, %edi
	loop 1b
	
	jmp default_entry


/*
 * Non-boot CPU(AP) entry point.
 * Be here in 16-bit mode?
 */
ENTRY(startup_32_smp)
	xorl %eax, %eax
	movl $(__BOOT_DS), %eax
	movl %eax, %ss
	movl %eax, %ds
	movl %eax, %es
	movl %eax, %fs
	movl %eax, %gs

default_entry:
	# Enable Paging
	movl $pa(early_page_dir), %eax
	movl %eax, %cr3
	movl $CR0_STATE, %eax
	movl %eax, %cr0

	# Clear prefetch and normalize %eip after enable paging.
	#ljmp $__BOOT_CS, $1f
1:
	add $0, %esp				# Shift sp to a virtual address

	# Reload system registers
	lgdt pa(gdt_descr)
	lidt pa(idt_descr)
	
	# Normalize %eip after reloading
	ljmp $(__KERNEL_CS), $1f
1:
/*
 * FIXME
 * reload segment registers
 */
	movl $(__KERNEL_DS), %eax
	movl %eax, %ss
	movl %eax, %ds
	movl %eax, %es
	movl %eax, %gs

	movl $(__KERNEL_PERCPU), %eax
	movl %eax, %fs				# Set this cpu's percpu

	call kernel_init

die:
	hlt
	jmp die

/* Default interrupt handler does nothing. */
ENTRY(ignore_int)
	incb 0xb8000
	iret
ENDPROC(ignore_int)

	.section .data
	
	ALIGN
ENTRY(idt_descr)
	.word IDT_ENTRIES*8-1
	.long pa(idt_table)
	
ENTRY(gdt_descr)
	.word GDT_ENTRIES*8-1
	.long pa(gdt_table)
	
	.section .bss
	
	.align 4096
ENTRY(early_page_dir)
	.fill 1024,4,0
ENTRY(early_page_table)
	.fill 1024,4,0
