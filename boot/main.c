/*
 *	2015/04/11. Created by Shan Yizhou.
 *
 *	main.c: 16-bit image main body, is responsible for getting the
 *	system data from the BIOS, and putting them into the appropriate
 *	places in system memory. 
 *
 *	This code asks the bios for memory/disk/other parameters, and
 *	puts them in a "safe" place: 0x90000-0x901FF, ie where the
 *	boot-block used to be. It is then up to the protected mode
 *	system to read them from there before the area is overwritten
 *	for buffer-blocks.
 *	
 *	BIG FAT NOTE: We're in real mode using 64k segments.  Therefore segment
 *	addresses must be multiplied by 16 to obtain their respective linear
 *	addresses. To avoid confusion, linear addresses are written using leading
 *	hex while segment addresses are written as segment:offset.
 */
asm(".code16gcc");
void main(void){
 	asm(
		"movw $0x0007, %bx\n\t"
		"movw $0x0001, %cx\n\t"
		"movb $0x0e, %ah\n\t"
		"movb $66, %al\n\t"
		"int $0x10\n\t"
	);	
}
