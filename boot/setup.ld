/*
 *	2015/04/28 Created by Shan Yizhou
 *	setup.ld
 *	Linker script for Real-mode HEADER image
 */

SECTIONS
{
	. = 0;
	.text		: { *(.text) }
	.text32		: { *(.text32)}
	
	. = ALIGN(16);
	.data		: { *(.data*)}
	
	. = ALIGN(16);
	.rodata		: { *(.rodata*)}
	
	. = ALIGN(16);
	_end = .;
}
