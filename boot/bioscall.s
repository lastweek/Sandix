/* 
 * 2015/04/14 Created by Shan Yizhou.
 *
 * Copyright 2009-2014 Intel Corporation; author H. Peter Anvin
 *
 * bioscall.S: "Glove box" for BIOS calls.  Avoids the constant
 * problems with BIOSes touching registers they shouldn't be.
 */

	.code16
	.globl intcall
	.type intcall, @function
intcall:
	pushl %ebp
	movl %esp, %ebp
