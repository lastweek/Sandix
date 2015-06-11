/*
 * Protected-Mode Kernel Image Linker Scripts
 */

/* startup_32 located in init/head.S */
ENTRY(startup_32)

SECTIONS
{
	. = 0x0;
	.text	: { *(.text) }

	. = ALIGN(16);
	.data	: { *(.data*)}

	. = ALIGN(16);
	.rodata	: { *(.rodata*)}

	. = ALIGN(16);
	.bss		:
	{
		__bss_start = .;
		*(.bss)
		__bss_end = .;
	}
	
	. = ALIGN(16);
	PM_KERNEL_END = .;
}
