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

void initregs(struct biosregs *reg)
{	
	char *d = (char *)reg;
	size_t i = 0;

	while (i++ < sizeof(struct biosregs))
		*d++ = 0;

	reg->ds = ds();
	reg->es = ds();
	reg->fs = fs();
	reg->gs = gs();
	reg->eflags |= 1;/* Set CF */
}
