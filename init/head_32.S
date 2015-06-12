.text

/* Physical address */
#define pa(X) ((X) - __PAGE_OFFSET)

/*
 * This is how much memory in addition to the memory covered up to
 * and including _end we need mapped initially.
 * We need:
 *     (KERNEL_IMAGE_SIZE/4096) / 1024 pages (worst case, non PAE)
 *     (KERNEL_IMAGE_SIZE/4096) / 512 + 4 pages (worst case for PAE)
 *
 * Modulo rounding, each megabyte assigned here requires a kilobyte of
 * memory, which is currently unreclaimed.
 *
 * This should be a multiple of a page.
 *
 * KERNEL_IMAGE_SIZE should be greater than pa(_end)
 * and small than max_low_pfn, otherwise will waste some page table entries
 */

#if PTRS_PER_PMD > 1
#define PAGE_TABLE_SIZE(pages) (((pages) / PTRS_PER_PMD) + PTRS_PER_PGD)
#else
#define PAGE_TABLE_SIZE(pages) ((pages) / PTRS_PER_PGD)
#endif

/* Number of possible pages in the lowmem region */
LOWMEM_PAGES = (((1<<32) - __PAGE_OFFSET) >> PAGE_SHIFT)
	
/* Enough space to fit pagetables for the low memory linear map */
MAPPING_BEYOND_END = PAGE_TABLE_SIZE(LOWMEM_PAGES) << PAGE_SHIFT

/*
 * Worst-case size of the kernel mapping we need to make:
 * a relocatable kernel can live anywhere in lowmem, so we need to be able
 * to map all of lowmem.
 */
KERNEL_PAGES = LOWMEM_PAGES

INIT_MAP_SIZE = PAGE_TABLE_SIZE(KERNEL_PAGES) * PAGE_SIZE
RESERVE_BRK(pagetables, INIT_MAP_SIZE)

__HEAD


ENTRY(startup_32)

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

	jmp default_entry


/*
 * Non-boot CPU entry point; entered from trampoline.S
 * We can't lgdt here, because lgdt itself uses a data segment, but
 * we know the trampoline has already loaded the boot_gdt for us.
 *
 * If cpu hotplug is not supported then this code can go in init section
 * which will be freed later
 */
ENTRY(startup_32_smp)
	cld
	movl $(__BOOT_DS),%eax
	movl %eax,%ds
	movl %eax,%es
	movl %eax,%fs
	movl %eax,%gs
	movl pa(stack_start),%ecx
	movl %eax,%ss
	leal -__PAGE_OFFSET(%ecx),%esp


#define CR0_STATE
		(X86_CR0_PE | X86_CR0_MP | X86_CR0_ET | \
		 X86_CR0_NE | X86_CR0_WP | X86_CR0_AM | \
		 X86_CR0_PG)

default_entry:
	movl $(CR0_STATE & ~X86_CR0_PG),%eax
	movl %eax,%cr0

/*
 * We want to start out with EFLAGS unambiguously cleared. Some BIOSes leave
 * bits like NT set. This would confuse the debugger if this code is traced. So
 * initialize them properly now before switching to protected mode. That means
 * DF in particular (even though we have cleared it earlier after copying the
 * command line) because GCC expects it.
 */
	pushl $0
	popfl

/*
 * New page tables may be in 4Mbyte page mode and may be using the global pages.
 */
enable_paging:

	movl $pa(initial_page_table), %eax
	movl %eax,%cr3		/* set the page table pointer.. */
	movl $CR0_STATE,%eax
	movl %eax,%cr0		/* ..and set paging (PG) bit */
	ljmp $__BOOT_CS,$1f	/* Clear prefetch and normalize %eip */
1:
	/* Shift the stack pointer to a virtual address */
	addl $__PAGE_OFFSET, %esp

/*
 * start system 32-bit setup. We need to re-do some of the things done
 * in 16-bit mode for the "real" operations.
 */
	movl setup_once_ref,%eax
	andl %eax,%eax
	jz 1f				# Did we do this already?
	call *%eax
1:
	movl $0x50022,%ecx	# set AM, WP, NE and MP
	movl %cr0,%eax
	andl $0x80000011,%eax	# Save PG,PE,ET
	orl %ecx,%eax
	movl %eax,%cr0

	lgdt early_gdt_descr
	lidt idt_descr
	ljmp $(__KERNEL_CS),$1f
1:	movl $(__KERNEL_DS),%eax	# reload all the segment registers
	movl %eax,%ss			# after changing gdt.

	movl $(__USER_DS),%eax		# DS/ES contains default USER segment
	movl %eax,%ds
	movl %eax,%es

	movl $(__KERNEL_PERCPU), %eax
	movl %eax,%fs			# set this cpu's percpu


	xorl %eax,%eax			# Clear LDT
	lldt %ax

	pushl $0		# fake return address for unwinder
	jmp *(initial_code)


/*
 *  setup_once
 *
 *  The setup work we only want to run on the BSP.
 *
 *  Warning: %esi is live across this function.
 */
__INIT
setup_once:
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

	andl $0,setup_once_ref	/* Once is enough, thanks */
	ret

ENTRY(early_idt_handlers)
	# 36(%esp) %eflags
	# 32(%esp) %cs
	# 28(%esp) %eip
	# 24(%rsp) error code
	i = 0
	.rept NUM_EXCEPTION_VECTORS
	.if (EXCEPTION_ERRCODE_MASK >> i) & 1
	ASM_NOP2
	.else
	pushl $0		# Dummy error code, to make stack frame uniform
	.endif
	pushl $i		# 20(%esp) Vector number
	jmp early_idt_handler
	i = i + 1
	.endr
ENDPROC(early_idt_handlers)
	
	/* This is global to keep gas from relaxing the jumps */
ENTRY(early_idt_handler)
	cld

	cmpl $2,(%esp)		# X86_TRAP_NMI
	je is_nmi		# Ignore NMI

	cmpl $2,%ss:early_recursion_flag
	je hlt_loop
	incl %ss:early_recursion_flag

	push %eax		# 16(%esp)
	push %ecx		# 12(%esp)
	push %edx		#  8(%esp)
	push %ds		#  4(%esp)
	push %es		#  0(%esp)
	movl $(__KERNEL_DS),%eax
	movl %eax,%ds
	movl %eax,%es

	cmpl $(__KERNEL_CS),32(%esp)
	jne 10f

	leal 28(%esp),%eax	# Pointer to %eip
	call early_fixup_exception
	andl %eax,%eax
	jnz ex_entry		/* found an exception entry */

10:
	call dump_stack
hlt_loop:
	hlt
	jmp hlt_loop

ex_entry:
	pop %es
	pop %ds
	pop %edx
	pop %ecx
	pop %eax
	decl %ss:early_recursion_flag
is_nmi:
	addl $8,%esp		/* drop vector number and error code */
	iret
ENDPROC(early_idt_handler)

	ALIGN
ignore_int:
	cld
	iret
ENDPROC(ignore_int)

__INITDATA
	.align 4
early_recursion_flag:
	.long 0

__REFDATA
	.align 4
ENTRY(initial_code)
	.long i386_start_kernel
ENTRY(setup_once_ref)
	.long setup_once

/*
 * BSS section
 */
__PAGE_ALIGNED_BSS
	.align PAGE_SIZE
ENTRY(initial_page_table)
	.fill 1024,4,0
initial_pg_fixmap:
	.fill 1024,4,0
ENTRY(empty_zero_page)
	.fill 4096,1,0
ENTRY(swapper_pg_dir)
	.fill 1024,4,0

.data
.balign 4
ENTRY(stack_start)
	.long init_thread_union+THREAD_SIZE

__INITRODATA
int_msg:
	.asciz "Unknown interrupt or fault at: %p %p %p\n"

fault_msg:
/* fault info: */
	.ascii "BUG: Int %d: CR2 %p\n"
/* regs pushed in early_idt_handler: */
	.ascii "     EDI %p  ESI %p  EBP %p  EBX %p\n"
	.ascii "     ESP %p   ES %p   DS %p\n"
	.ascii "     EDX %p  ECX %p  EAX %p\n"
/* fault frame: */
	.ascii "     vec %p  err %p  EIP %p   CS %p  flg %p\n"
	.ascii "Stack: %p %p %p %p %p %p %p %p\n"
	.ascii "       %p %p %p %p %p %p %p %p\n"
	.asciz "       %p %p %p %p %p %p %p %p\n"

#include "../../x86/xen/xen-head.S"

/*
 * The IDT and GDT 'descriptors' are a strange 48-bit object
 * only used by the lidt and lgdt instructions. They are not
 * like usual segment descriptors - they consist of a 16-bit
 * segment size, and 32-bit linear address value:
 */

	.data
.globl boot_gdt_descr
.globl idt_descr

	ALIGN
# early boot GDT descriptor (must use 1:1 address mapping)
	.word 0				# 32 bit align gdt_desc.address
boot_gdt_descr:
	.word __BOOT_DS+7
	.long boot_gdt - __PAGE_OFFSET

	.word 0				# 32-bit align idt_desc.address
idt_descr:
	.word IDT_ENTRIES*8-1		# idt contains 256 entries
	.long idt_table

# boot GDT descriptor (later on used by CPU#0):
	.word 0				# 32 bit align gdt_desc.address
ENTRY(early_gdt_descr)
	.word GDT_ENTRIES*8-1
	.long gdt_page			/* Overwritten for secondary CPUs */

/*
 * The boot_gdt must mirror the equivalent in setup.S and is
 * used only for booting.
 */
	.align L1_CACHE_BYTES
ENTRY(boot_gdt)
	.fill GDT_ENTRY_BOOT_CS,8,0
	.quad 0x00cf9a000000ffff	/* kernel 4GB code at 0x00000000 */
	.quad 0x00cf92000000ffff	/* kernel 4GB data at 0x00000000 */
