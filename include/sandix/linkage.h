#ifndef _SANDIX_LINKAGE_H
#define _SANDIX_LINKAGE_H

#define asmlinkage __atrribute__((regparm(0)))

#define ASM_NL	;

#define GLOBAL(name) \
		.globl name ASM_NL \
		name:

#define ENTRY(name) \
		.globl name ASM_NL \
		.align 4 ASM_NL \
		name:

#define END(name) \
		.size name, .-name

#define ENDPROC(name) \
		.type name, @function ASM_NL \
		END(name)

#define WEAK(name) \
		.weak name ASM_NL \
		name:

#endif /* _SANDIX_LINKAGE_H */
