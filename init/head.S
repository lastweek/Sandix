/*
 *	2015/04/29 Created by Shan Yizhou.
 *
 *	head.S: 32-bit kernel entrypoint; only used by the boot CPU.
 *	On entry, %esi points to the real-mode code as a 32-bit pointer.
 *	CS and DS must be 4 GB flat segments.
 */

 .code32

_start:
	# Print sth on screen, '@' in left-upper corner.
	# Show we are here.
	movl $0xB8000, %ebx
	movb $0x40, (%ebx)

	# Call Kernel Init. Never Return.
	calll kernel_init

end:
	hlt
	jmp end
