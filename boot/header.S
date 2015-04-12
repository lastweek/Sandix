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
	.space 512
	
###########################
# OFFSET 512, ENTRY POINT #
###########################
	.global _start
_start:
	jmp start_of_setup
###########################
# START of setup header   #
###########################
header:
	.ascii "SANDIX15"		# header signature
	.word 0x0001			# header version number 0.1
code32_start:
	.long 0x100000			# 0x100000 = default for kernel
###########################
# END   of setup header   #
###########################

start_of_setup:
	movw $INITSEG, %ax
	movw %ax, %ds
	movw %ax, %es
	
	movb $0x03, %ah	# read cursor pos
	xor %bh, %bh
	int $0x10
	
	movw $(end-fail_msg), %cx # length of msg.
	movw $0x0007, %bx # page 0, attribute 7.
	movw $start_msg, %bp # pointer to msg.
	movw $0x1301, %ax # write msg, move cursor.
	int $0x10
die:
	hlt
	jmp die
fail_msg:
	.ascii "\n\rSetup signature missing/mismatch..."
end:
	nop
