/**
 *	head.S --- Protected-Mode Kernel Image Entrypoint.
 *
 *	Here we enable paging, init CR3, reload GDTR & IDTR on
 *	boot cpu(BP, boot processor). Asides, ignore APIC or i8259
 *	for now, we initialize them later. After all this, control
 *	transferred to kernel_init() which do residual work.
 *
 *	NOTE: We should finish initializing the boot cpu at first palce!
 *	Until everything is settled then we can talk about SMP, right?
 **/

#include <sandix/linkage.h>

	.code32
	.text
	.globl kernel_start
kernel_start:
	
	# Print '@' in left-upper corner.
	# Show we are here.
	movl $0xB8000, %ebx
	movb $0x40, (%ebx)

	# Already have a valid %esp
	# TODO




	call kernel_init
end:
	hlt
	jmp end


ENTRY(early_idt_handlers)
	#TODO
ENDPROC(early_idt_handlers)

ENTRY(early_idt_handler)
	#TODO
ENDPROC(early_idt_handler)


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
