#include <sandix/linkage.h>
#include <asm/segment.h>
#include <asm/bootparam.>

#define KERNEL_OFFSET	0x00000000
#define pa(X)	((X) - KERNEL_OFFSET)

#define PG	0x80000000
#define MP  0x00000002
#define PE	0x00000001
#define CR0_STATE	PG|MP|PE

	.code32
	.section .text

/*
 *	32-bit kernel entrypoint. Only used by the boot CPU(BP).
 *	startup_32 initialize page directory, page table and
 *	interrupt descriptor table(idt), which have exact only
 *	one copy in memory respectively. Caveat: paging and
 *	lgdt and lidt must be done on each CPU.
 *
 *	Also, on entry, %esi points to the boot_params struct.
 */
ENTRY(startup_32)

	# DEBUG
	# Show '@' in left-upper corner.
	movl $0xb8000, %ebx
	movb $0x40, (%ebx)

	# Clear BSS to avoid abnormal things.
 	cld
	xorl %eax, %eax
	movl $pa(__bss_start), %edi
	movl $pa(__bss_end), %ecx
	subl %edi, %ecx
	shrl $2, %ecx
	rep ; stosl

	# Move boot_param to a harbor

/*
 * Initialize early_page_directory and early_page_table.
 * Only one page table is present, but 4M is enough before
 * we do memory initialization.
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
	movl $(__KERNEL_DS), %eax	# Reload all segment registers
	movl %eax, %ss
	movl %eax, %ds
	movl %eax, %es

	movl $(__KERNEL_PERCPU), %eax
	movl %eax, %fs				# Set this cpu's percpu

	call kernel_init

die:
	hlt
	jmp die

/**
 * Default interrupt handler does nothing.
 **/
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
