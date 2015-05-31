#
#	head.S --- 32-bit Kernel Image Entrypoint.
#	Based on linux/arch/x86/kernel/head_32.S
#

#include <sandix/linkage.h>
#include <sandix/segment.h>

.code32
.text

/**
 *	32-bit kernel entrypoint; only used by the boot CPU.  On entry,
 *	%esi points to the real-mode code as a 32-bit pointer.
 *	CS and DS must be 4 GB flat segments, but we don't depend on
 *	any particular GDT layout, coz we load our own as soon as we can.
 **/
ENTRY(startup_32)
	
	# '@' in left-upper corner.
	# Show we are here.
	movl $0xb8000, %ebx
	movb $0x40, (%ebx)

/**
 *	Clear BSS first
 *	so that there are no surprises...
 **/

/**
 *	Initialize page tables.
 **/

/**
 *	Initialize IDT and GDT
 **/

die:
	hlt
	jmp die


ENTRY(early_idt_handlers)
	movl %eax, %ebx
	movl %eax, %ebx
ENDPROC(early_idt_handlers)


ENTRY(early_idt_handler)
	#TODO
	movl %eax, %ebx
ENDPROC(early_idt_handler)


ENTRY(ignore_int)
	iret
ENDPROC(ignore_int)

/*
 * The IDT and GDT 'descriptors' are a strange 48-bit object
 * only used by the lidt and lgdt instructions. They are not
 * like usual segment descriptors - they consist of a 16-bit
 * segment size, and 32-bit linear address value:
 *
 * idt_table and gdt_table are initialized on init/init.c
 * early_gdt/idt_descr are used by boot cpu only.
 */
	.data
	.globl early_gdt_descr
	.globl early_idt_descr

	.align 16

early_idt_descr:
	.word IDT_ENTRIES*8-1
	.long idt_table
	
early_gdt_descr:
	.word GDT_ENTRIES*8-1
	.long gdt_table
