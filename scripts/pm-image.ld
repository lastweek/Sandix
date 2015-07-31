/*
 *	Sandix Linker Script.
 *
 *	Copyright (C) 2015 Yizhou Shan
 *
 *	Note that linker can do many many things. So many Linux kernel tricks
 *	rely on linker, such as put the same sections together, export the final
 *	image size and so on. Sandix rely on these stuff, too. It is simple,
 *	but really useful.
 */

/*
 * Physical address offset.
 * Since we put our image at 0x10000.
 * FIXME Should put it at 0x100000.
 */
PHYS_LOAD_OFFSET = 0x10000;

/*
 * Kernel Page Offset.
 * In 32-bit system, this is 0xC0000000, which is the PAGE_OFFSET.
 * And that's why we have NORMAL_MEM and HIGH_MEM! I really confused about why
 * just do not using 0x0 as kernel virtual address basis? Then kernel do not
 * have to distinguish between normal and high memory!
 */
LOAD_OFFSET = 0x0;

ENTRY(startup_32)
SECTIONS
{
	. = PHYS_LOAD_OFFSET + LOAD_OFFSET;
	
	.text	: { *(.text) }
	__end_text = .;

	. = ALIGN(16);
	.data	: { *(.data*)}

	. = ALIGN(16);
	.rodata	: { *(.rodata*)}
	__end_data = .;

	. = ALIGN(16);
	.bss		:
	{
		__bss_start = .;
		*(.bss)
		__bss_end = .;
	}
	
	/DISCARD/ : { *(.note*) }
	/DISCARD/ : { *(.eh_frame) }
	/DISCARD/ : { *(.comment) }
	
	. = ALIGN(16);
	__end = .;
}
