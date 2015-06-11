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

	# Clear BSS
 	cld
	xorl %eax, %eax
	movl $__bss_start, %edi
	movl $__bss_end, %ecx
	subl %edi, %ecx
	shrl $2, %ecx
	rep ; stosl

/*
 * Initialize page tables.  This creates a PDE and a set of page
 * tables, which are located immediately beyond __brk_base.  The variable
 * _brk_end is set up to point to the first "safe" location.
 * Mappings are created both at virtual address 0 (identity mapping)
 * and PAGE_OFFSET for up to _end.
 */
page_pde_offset = (__PAGE_OFFSET >> 20);

	movl $pa(__brk_base), %edi
	movl $pa(initial_page_table), %edx
	movl $PTE_IDENT_ATTR, %eax
10:
	leal PDE_IDENT_ATTR(%edi),%ecx		/* Create PDE entry */
	movl %ecx,(%edx)			/* Store identity PDE entry */
	movl %ecx,page_pde_offset(%edx)		/* Store kernel PDE entry */
	addl $4,%edx
	movl $1024, %ecx
11:
	stosl
	addl $0x1000,%eax
	loop 11b
	/*
	 * End condition: we must map up to the end + MAPPING_BEYOND_END.
	 */
	movl $pa(_end) + MAPPING_BEYOND_END + PTE_IDENT_ATTR, %ebp
	cmpl %ebp,%eax
	jb 10b
	addl $__PAGE_OFFSET, %edi
	movl %edi, pa(_brk_end)
	shrl $12, %eax
	movl %eax, pa(max_pfn_mapped)

	/* Do early initialization of the fixmap area */
	movl $pa(initial_pg_fixmap)+PDE_IDENT_ATTR,%eax
	movl %eax,pa(initial_page_table+0xffc)

/*
 * Set up a idt with 256 entries pointing to ignore_int,
 * interrupt gates. It doesn't actually load idt - that needs
 * to be done on each CPU. Interrupts are enabled elsewhere,
 * when we can be relatively sure everything is ok.
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
ENTRY(early_idt_descr)
	.word IDT_ENTRIES*8-1
	.long idt_desc
	
ENTRY(early_gdt_descr)
	.word GDT_ENTRIES*8-1
	.long gdt_desc
