/*
 *	2015/04/15 Created by Shan Yizhou.
 *	Copyright 2009 Intel Corporation; author H. Peter Anvin
 *
 *	regs.c: Helper function for initializing a register set.
 *
 *	Note by author: this sets EFLAGS_CF in the input register set;
 *	this makes it easier to catch functions which do nothing but
 *	don't explicitly set CF.
 *
 *	Note by Shan Yizhou: I donot know which BIOS call will do such
 *	things as described above, but i guess it's better to set CF in
 *	case some terrible things come out.
 */

#include "boot.h"
#include "string.h"

asm(".code16gcc");

void initregs(struct biosregs *reg)
{
	memset(reg, 0, sizeof(struct biosregs));
	reg->ds = ds();
	reg->es = ds();
	reg->fs = fs();
	reg->gs = gs();
	
	reg->eflags |= 1;
	//reg->eflags |= X86_EFLAGS_CF;
}
