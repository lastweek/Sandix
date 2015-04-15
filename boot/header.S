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
	# Here, stack_addr = %ss<<4:%sp, %ss=0x9000
	# The region (0x9000-end) belongs to header.s. The bad news is
	# now we donot konw how much space main() will use. It means
	# we have no idea what is the upper bound of 16-bit setup-image.
	# So, just give a shot, set %sp=0x0000, which means the stack
	# point now is locate at 0xA000, just below Video region.
	movw $0x800, %sp

	# Now call C function.
	call main
die:
	hlt
	jmp die
Debug_msg:
	.ascii "\n\rDebug: Now we are in header.s, going to main()"
fail_msg:
	.ascii "\n\rHeader.s signature missing/mismatch..."
end:
	nop
