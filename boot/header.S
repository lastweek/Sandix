#########################################################################
#	2015/04/11. Created by Shan Yizhou.
#
#	header.s: 16-bit image header, prepare to go to main();
#
#########################################################################
#
#	Based on Linux/x86/boot/header.S
#
#########################################################################
BOOTSEG = 0x07C0
INITSEG = 0X9000

	.code16
	.text
	.global header_start
header_start:
	# Information about system!
	# Filled by main()???
	# NEED FIXED
	.space 512
	
###########################
# OFFSET 512, ENTRY POINT #
###########################
	.global _start
_start:
	jmp start_of_setup
#------------------------------------------------------------#
# START of setup header										 #
#------------------------------------------------------------#
header:
	.ascii "SANDIX15"		# header signature
	.word 0x0001			# header version number 0.1
code32_start:
	.long 0x100000			# 0x100000 = default for kernel
#------------------------------------------------------------#
# END   of setup header										 #
#------------------------------------------------------------#
start_of_setup:
	# Print some message to indicate we are here.
	movw $INITSEG, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss

	movb $0x03, %ah	# read cursor pos
	xor %bh, %bh
	int $0x10
	movw $(fail_msg-Debug_msg), %cx # length of msg.
	movw $0x0007, %bx # page 0, attribute 7.
	movw $Debug_msg, %bp # pointer to msg.
	movw $0x1301, %ax # write msg, move cursor.
	int $0x10

	# Before we call main(), we must set up %sp first.
	# For now, we don't konw how much space main() will take.
	# It means we have no idea what is the upper bound of setup-image.

	# FIXME: Tentative %sp
	movw $0x3000, %sp

	# GO PARTY, never return!
	calll main
die:
	hlt
	jmp die
Debug_msg:
	.ascii "\n\rDebug: Now we are in header.s, heading to main()...\n\r"
fail_msg:
	.ascii "\n\rHeader.s signature missing/mismatch..."
end:
	hlt
	nop
