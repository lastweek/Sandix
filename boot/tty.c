/*
 * 2015/04/14 Created by Shan Yizhou.
 * 
 * tty.c: Simple screen output.
 */

/* Do we need to save and restore axbxcx????? */
void bios_putchar(int ch) {
 	asm(
		"movw $0x0007, %bx\n\t"
		"movw $0x0001, %cx\n\t"
		"movb $0x0e, %ah\n\t"
		"movb ch, %al\n\t"
		"int $0x10\n\t"
	);	
}
