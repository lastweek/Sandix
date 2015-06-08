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
 	cld
	xorl %eax, %eax
	movl $__bss_start, %edi
	movl $__bss_stop, %ecx
	subl %edi, %ecx
	shrl $2 %ecx
	rep ; stosl

	/**
	*	Initialize page tables.
	**/


	/**
	*	Initialize IDT and GDT
	**/

die:
	hlt
	jmp die


ENTRY(init_idt_desc)
	movl %eax, %ebx
	movl %eax, %ebx
ENDPROC(init_idt_desc)

ENTRY(init_gdt_desc)
	movl %eax, %ebx
ENDPROC(init_gdt_desc)


ENTRY(early_idt_handler)
	#TODO
	movl %eax, %ebx
ENDPROC(early_idt_handler)


ENTRY(ignore_int)
	iret
ENDPROC(ignore_int)

	.data

	/**
	* The IDT and GDT 'descriptors' are a strange 48-bit object
	* only used by the lidt and lgdt instructions. They are not
	* like usual segment descriptors - they consist of a 16-bit
	* segment size, and 32-bit linear address value:
	*
	* idt_table and gdt_table are initialized on init/init.c
	* early_gdt/idt_descr are used by boot cpu only.
	**/
	
	ALIGN
ENTRY(early_idt_descr):
	.word IDT_ENTRIES*8-1
	.long idt_table
	
ENTRY(early_gdt_descr):
	.word GDT_ENTRIES*8-1
	.long gdt_table
