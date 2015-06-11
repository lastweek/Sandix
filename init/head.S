#include <sandix/linkage.h>
#include <sandix/segment.h>

	.code32
	.text
	
/**
 *	32-bit kernel entrypoint.
 *	Only used by the boot CPU.
 *	Remember, startup_32 initialize page table and idt via boot CPU.
 *	However paging enable and lgdt and lidt must be done on each CPU.
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
 * Initialize early page directory and page tables.
 */
 	# FIXME
	movl early_page_dir, %cr3

/*
 * Setup idt pointing to ignore_int, interrupt gates.
 */
	movl $idt_table,%edi
	movl $early_idt_handlers,%eax
	movl $NUM_EXCEPTION_VECTORS,%ecx
1:
	movl %eax,(%edi)
	movl %eax,4(%edi)
	/* interrupt gate, dpl=0, present */
	movl $(0x8E000000 + __KERNEL_CS),2(%edi)
	addl $9,%eax
	addl $8,%edi
	loop 1b

	movl $256 - NUM_EXCEPTION_VECTORS,%ecx
	movl $ignore_int,%edx
	movl $(__KERNEL_CS << 16),%eax
	movw %dx,%ax		/* selector = 0x0010 = cs */
	movw $0x8E00,%dx	/* interrupt gate - dpl=0, present */
2:
	movl %eax,(%edi)
	movl %edx,4(%edi)
	addl $8,%edi
	loop 2b
	
	jmp default_entry


/*
 * startup_32_smp:
 * Non-boot CPU entry point.
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

ENTRY(idt_descr)
	.word IDT_ENTRIES*8-1
	.long idt_desc
	
ENTRY(gdt_descr)
	.word GDT_ENTRIES*8-1
	.long gdt_desc


	.align 4096
ENTRY(early_page_dir):
	.fill 1024,4,0
ENTRY(early_page_table):
	.fill 1024,4,0
