/*
 * 2015/04/15 Created by Shan Yizhou.
 *
 * Copyright 2009 Intel Corporation; author H. Peter Anvin
 *
 * regs.c: Helper function for initializing a register set.
 */

#include "boot.h"
#include "string.h"

asm(".code16");
/*
 * Note that this sets EFLAGS_CF in the input register set; this
 * makes it easier to catch functions which do nothing but don't
 * explicitly set CF.
 */

/*
 * sizeof *reg =--> sizeof(struct biosregs)
 * Example:
 * char *c; sizeof(*c) =--> 1; sizeof(char *) =--> 4;
 */
void initregs(struct biosregs *reg)
{
	memset(reg, 0, sizeof *reg);
	//reg->eflags |= X86_EFLAGS_CF;
	reg->eflags |= 1;
	reg->ds = ds();
	reg->es = ds();
	reg->fs = fs();
	reg->gs = gs();
}
