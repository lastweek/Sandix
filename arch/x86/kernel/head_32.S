#	Copyright (C) 2015-2016 Yizhou Shan <shan13@purdue.edu>
#
#	This program is free software; you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation; either version 2 of the License, or
#	(at your option) any later version.
#
#	This program is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License along
#	with this program; if not, write to the Free Software Foundation, Inc.,
#	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#include <asm/page.h>
#include <asm/segment.h>
#include <asm/bootparam.h>

#include <sandix/linkage.h>
#include <asm/processor-flags.h>

#define pa(X)	((X) - __PAGE_OFFSET)

	.code32
	.section .text

# Boot-CPU entry point
ENTRY(startup_32)

# Red '@' left-upper corner.
# Show we are here
	movl	$0xb8000, %ebx
	movw	$0x0440, (%ebx)

# Check signature at end of kernel
# Quick check if entire image is loaded
	cmpl	$0x5a5aaa55, pa(kernel_sig)
	jne	bad_sig

# Reload segment registers. We have done this before,
# but we do it again here just for safey.
	movl	$__BOOT_DS, %eax
	movl	%eax, %ds
	movl	%eax, %ss
	movl	%eax, %es

# Clear BSS to avoid abnormal things.
	cld
	xorl	%eax, %eax
	movl	$pa(__bss_start), %edi
	movl	$pa(__bss_end), %ecx
	subl	%edi, %ecx
	shrl	$2, %ecx
	rep
	stosl

# Copy setup parameters to a safe place.
# Note that %esi still points to struct boot_param in real mode.
# The size of struct boot_param is 4096 bytes.
	movl	$pa(boot_params), %edi
	movl	$BOOT_PARAMS_SIZE, %ecx
	shrl	$2, %ecx
	rep
	movsl

# Initialize page directory and page tables. We do NOT know
# how many page tables we need during build, so we use memory
# beyond brk to store as many page tables as kernel needs.
# (For now, brk is 64KB == 16 page tables == 64M mapping)
#
# Mapping are created for virtual address 0(identical mapping)
# and PAGE_OFFSET(kernel mapping) repectively.
#
# PTE_ATTR: PRESENT, READ/WRITE
# PDE_ATTR: PRESENT, READ/WRITE, DIRTY, ACCESSED
# PDE_OFFSET: Kernel PDE entry, >> 22, << 2(4 bytes align)

#define PTE_ATTR	0x003
#define PDE_ATTR	0x063
#define PDE_OFFSET	(__PAGE_OFFSET >> 20)

	movl	$pa(__brk_start), %edi		# .brk section that follow .bss
 	movl	$pa(initial_page_table), %ebx	# Top page directory
	movl	$PTE_ATTR, %eax
10:
	leal	PDE_ATTR(%edi), %ecx		# Create PDE entry
	movl	%ecx, (%ebx)			# Store identical PDE entry
	movl	%ecx, PDE_OFFSET(%ebx)		# Store kernel PDE entry
	addl	$4, %ebx			# Next PDE entry if needed
	movl	$1024, %ecx
11:
	stosl
	addl	$0x1000, %eax
	loop	11b

	movl	$pa(__kend) + PTE_ATTR, %ecx	# Check if need another PDE
	cmpl	%ecx, %eax			# Note that, %edi points to next
	jb	10b				# page table automatically.

	addl	$__PAGE_OFFSET, %edi
	movl	%edi, pa(brk_end)		# Store page table upper limit
	shrl	$12, %eax
	movl	%eax, pa(max_pfn_mapped)	# Store maximum mapped frame

						# Create PDE for fixmap
	movl	$pa(initial_pg_fixmap)+PDE_ATTR, %eax
	movl	%eax, pa(initial_page_table + 0xffc)

# Set up a IDT with 256 interrupt gates that does nothing.
# We do NOT load IDT here - that need to be done on each cpu.
# Interrupts are enabled later, when we sure everything is ok.
# GATE_ATTR: Interrupt Gate + DPL=0 + Present

#define GATE_ATTR	0x8e000000

	movl	$pa(idt_table), %edi
	movl	$ignore_int, %eax
	movl	$IDT_ENTRIES, %ecx
1:
	movl	%eax, (%edi)
	movl	%eax, 4(%edi)
	movl	$(GATE_ATTR + __KERNEL_CS), 2(%edi)
	addl	$8, %edi
	loop	1b
	
	jmp	default_entry

# Non-boot CPU entry point.
# For future SMP and CPU-hotplug support
ENTRY(startup_32_smp)
	movl	$__BOOT_DS, %eax
	movl	%eax, %ss
	movl	%eax, %ds
	movl	%eax, %es
	movl	%eax, %fs
	movl	%eax, %gs

default_entry:
	
#define CR0_STATE	(X86_CR0_PE | X86_CR0_MP | X86_CR0_ET | \
			 X86_CR0_NE | X86_CR0_WP | X86_CR0_AM | \
			 X86_CR0_PG)

	pushl	$0
	popfl					# Clear EFLAGS

	movl	$pa(initial_page_table), %eax
	movl	%eax, %cr3			# Store the page table pointer
	movl	$CR0_STATE, %eax
	movl	%eax, %cr0			# Enable paging and etc.
	ljmp	$__BOOT_CS, $1f			# Clear prefetch and normalize %eip
1:	add	$__PAGE_OFFSET, %esp		# Shift %esp to virtual address

	lgdt	pa(gdt_descr)
	lidt	pa(idt_descr)
	ljmp	$__KERNEL_CS, $1f		# Clear prefetch and normalize %eip
1:	movl	$__KERNEL_DS, %eax
	movl	%eax, %ss			# Reload all segment registers

	movl	$__USER_DS, %eax
	movl	%eax, %ds
	movl	%eax, %es			# WHY? FIXME
	movl	%eax, %gs			# Default USER segment

	movl	$__KERNEL_PERCPU, %eax
	movl	%eax, %fs			# Set this CPU's PERCPU

	xorl	%eax, %eax
	lldt	%ax				# Clear LDT

	call	start_kernel

bad_sig:
# Simple message
# Show a red 'B' after '@'
	movl	$0xb8002, %ebx
	movw	$0x0442, (%ebx)
	movw	$0x0461, 2(%ebx)
	movw	$0x0464, 4(%ebx)
1:	hlt
	hlt
	jmp 1b


ENTRY(ignore_int)
	incb	0xb8000
	iret
ENDPROC(ignore_int)

	.section .data
	.align 4
ENTRY(idt_descr)
	.word	IDT_ENTRIES*8-1
	.long	pa(idt_table)
	
ENTRY(gdt_descr)
	.word	GDT_ENTRIES*8-1
	.long	pa(gdt_table)

	.section .bss
	.align PAGE_SIZE
ENTRY(initial_page_table)
	.fill	1024,4,0
ENTRY(initial_pg_fixmap)
	.fill	1024,4,0
