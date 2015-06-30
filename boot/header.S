#########################################################################
#	2015/04/11. Created by Shan Yizhou.
#	Based on Linux/x86/boot/header.S
#
#	header.S: 16-bit image header.
#	The first thing is moving Protected-mode kernel image to 0x100000.
#	After that, this code will set up the %sp, then transfer to main()
#	who is responsible for other dirty work.
#
#
#	05/17
#	Maybe we can use grub or lilo to boot! In that way we can put our
#	kernel image in /boot directory, let grub to choose boot from Sandix
#	or the original Linux.
#	It's a better idea!!!!!
#
#	FIXME!!!!!
#########################################################################

BOOTSEG = 0x07C0
INITSEG = 0X9000

	.code16
	.text
	.global header_start
header_start:
	
	# For future use...
	.space 512
	
#------------------------------------------------------------
# OFFSET 512, ENTRY POINT
	.global _start
_start:
	jmp start_of_setup

#------------------------------------------------------------
# START of setup_header
	
	.globl hdr
hdr:
header_version:
	.long 0x1				# header version number 0.0.0.1
code32_start:
	.long 0x10000			# Entry of Protected-Mode Kernel
video_mode:
	.long 0x3				# Video mode that kernel supports
rm_kernel_sects:
	.long 0x0				# Sections of RM Kernel Image
pm_kernel_sects:
	.long 0x0				# Sections of PM kernel Image
header_signature:
	.ascii "_SANDIX_"		# header signature

# END   of setup_header
#------------------------------------------------------------

# Real start entry point
start_of_setup:
	# Print some message to indicate we are here.
	movw $INITSEG, %ax
	movw %ax, %ds
	movw %ax, %ss
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs

	movb $0x03, %ah	# read cursor pos
	xor %bh, %bh
	int $0x10
	movw $(end-Debug_msg), %cx # length of msg.
	movw $0x0007, %bx # page 0, attribute 7.
	movw $Debug_msg, %bp # pointer to msg.
	movw $0x1301, %ax # write msg, move cursor.
	int $0x10

	# FIXME: Tentative %sp
	movw $0x3000, %sp

	# Zero the bss
	# __bss_start & __bss_end defined in linker scripts
    movw    $__bss_start, %di 
	movw    $__bss_end, %cx 
	xorl    %eax, %eax
	subw    %di, %cx 
	shrw    $2, %cx 
	rep
	stosl
	
	# Go to RM kernel, should not return ...
	calll main
die:
	hlt
	jmp die
Debug_msg:
	.ascii "\n\rDEBUG: RM header.S, going to RM main() ...\n\r"
end:
	hlt
	nop
