/**
 *	Real-Mode Kernel Image Linker Scripts
 **/

/* _start(boot/header.S) */
ENTRY(_start)
SECTIONS
{
	. = 0x0;
	.text		: { *(.text) }
	.text32		: { *(.text32)}
	
	. = ALIGN(16);
	.data		: { *(.data*) }
	
	. = ALIGN(16);
	.rodata		: { *(.rodata*) }
	
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
