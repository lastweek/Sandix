 .code32
 .text

/*
 * 32-bit kernel entrypoint; only used by the boot CPU.  On entry,
 * %esi points to the real-mode code as a 32-bit pointer.
 * CS and DS must be 4 GB flat segments, but we don't depend on
 * any particular GDT layout, because we load our own as soon as we
 * can.
 */
_start:
	
	# Print '@' in left-upper corner.
	# Show we are here.
	movl $0xB8000, %ebx
	movb $0x40, (%ebx)

	# Already have a valid %esp

	# Call kernel_init, should not return...
	calll kernel_init

end:
	hlt
	jmp end
