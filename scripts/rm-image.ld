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
	. = ALIGN(16);
	RM_KERNEL_END = .;
}
