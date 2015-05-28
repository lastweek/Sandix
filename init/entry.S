/**
 *	entry.S
 *	Various entry points for traps or interrupts or else.
 **/

#include <sandix/linkage.h>

ENTRY(SAVE_ALL)
	#TODO
	push %eax
	push %ebx
	push %ecx
	push %edx
END(SAVE_ALL)

ENTRY(RESTORE_ALL)
	#TODO
	push %eax
	push %ebx
	push %ecx
	push %edx
END(RESTORE_ALL)
