#########################################################################
#	2015/04/11. Created by Shan Yizhou.
#
#	header.s: 16-bit image header, prepare to transfer to main();
#
#########################################################################

BOOTSEG = 0x07C0
INITSEG = 0X9000

	.code16
	.section .text

	.global header_start, start_of_setup
header_start:

start_of_setup:
