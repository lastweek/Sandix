#include <sandix/linkage.h>
#include <sandix/segment.h>

	.code32
	.text
	
/**
 *	32-bit kernel entrypoint. Only used by the boot CPU(BP).
 *
 *	Remember, startup_32 initialize page directory, page table and
 *	interrupt descriptor table(idt), which only have exact one copy
 *	in memory respectively via boot CPU. Caveat: paging enable and
 *	lgdt and lidt must be done on each CPU.
 *
 *	Also, on entry, %esi points to the boot_params struct as a pointer.
 **/
ENTRY(startup_32)
	
	# show '@' in left-upper corner.
	movl $0xb8000, %ebx
	movb $0x40, (%ebx)

/*
 * Clear BSS to avoid abnormal things.
 */
 	cld
	xorl %eax, %eax
	movl $__bss_start, %edi
	movl $__bss_end, %ecx
	subl %edi, %ecx
	shrl $2, %ecx
	rep ; stosl

/*
 * Initialize early_page_directory and early_page_table.
 * For now, only one page_table is present, which means
 * kernel image is confined beyond 4 MB.
 * Fix this later with ld script variable.
 */
 	movl early_page_dir, %eax
	movl early_page_table+7, (%eax)		#PRESENT R/W U/S
	
	movl early_page_table, %edi
	movl $7, %eax		#PRESENT R/W U/S
	movl $1024, %ecx
1:
	stosl
	addl 0x1000, %eax
	loop 1b

/*
 * Setup idt entries. We don't load idt here - that
 * need to be done on each cpu. Interrupts are enabled
 * elsewhere, when we sure everything is ok!!
 */
	movl $idt_table, %edi
	movl $ignore_int, %eax
	movl $idt_entries, %ecx
1:
	movl %eax, (%edi)
	movl %eax, 4(%edi)
	/* interrupt gate, dpl=0, present */
	movl $(0x8E000000 + __KERNEL_CS), 2(%edi)
	addl $8, %edi
	loop 1b
	
/*
 * Now go to default_entry prepared for all cpus!
 */
	jmp default_entry

/*
 * Non-boot CPU(AP) entry point.
 */
ENTRY(startup_32_smp)

default_entry:

die:
	hlt
	jmp die


# An default interrupt handler does nothing.
ENTRY(ignore_int)
	incb 0xb8000
	iret
ENDPROC(ignore_int)

	.data

ENTRY(lidt_descr)
	.word IDT_ENTRIES*8-1
	.long idt_desc
	
ENTRY(lgdt_descr)
	.word GDT_ENTRIES*8-1
	.long gdt_desc


	.align 4096
ENTRY(early_page_dir)
	.fill 1024,4,0
ENTRY(early_page_table)
	.fill 1024,4,0
