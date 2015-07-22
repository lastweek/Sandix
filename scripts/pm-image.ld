/* startup_32(init/head.S) */
ENTRY(startup_32)

SECTIONS
{
	/* Notice: We set virtual address start
	   from 0x10000, because the segment offset
	   is 0x10000. In the future, the start address
	   should plus PAGE_OFFSET */
	. = 0x10000;
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
