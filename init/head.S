/*
 *	2015/04/29 Created by Shan Yizhou.
 *
 *	head.S: 32-bit kernel entrypoint; only used by the boot CPU.
 *	On entry, %esi points to the real-mode code as a 32-bit pointer.
 *	CS and DS must be 4 GB flat segments, but we don't depend on any
 *	particular GDT layout, because we load our own as soon as we can.
 */

 .code32

pm_kernel_entry:
	# Print sth on screen, '@' in left-upper corner.
	# Show we are here.
	movl $0xB8000, %ebx
	movl $0x40, (%ebx)

	# Call Kernel Init. Never Return.
	calll kernel_init
head_end:
	nop
