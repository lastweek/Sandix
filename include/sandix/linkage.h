#ifndef _SANDIX_LINKAGE_H_
#define _SANDIX_LINKAGE_H_

/**
 * <sandix/linkage.h>:
 * Generic macros for assembly and C attributes.
 **/

#define asmlinkage __atrribute__((regparm(0)))

/* GNU AS newline separator */
#define	ASM_NL	;

/* Pad with NOP in x86 */
#ifndef ALIGN
#define ALIGN \
		.align 4, 0x90
#endif

#ifndef WEAK
#define WEAK(name) \
		.weak name ASM_NL \
		name:
#endif

#ifndef GLOBAL
#define GLOBAL(name) \
		.globl name ASM_NL \
		name:
#endif

#ifndef ENTRY
#define ENTRY(name) \
		.globl name ASM_NL \
		ALIGN ASM_NL \
		name:
#endif

#ifndef END
#define END(name) \
		.size name, .-name
#endif

/**
 *	If symbol 'name' is treated as a subroutine (gets called, and returns)
 *	then please use ENDPROC to mark 'name' as STT_FUNC.(For future use...)
 **/
#ifndef ENDPROC
#define ENDPROC(name) \
		.type name, @function ASM_NL \
		END(name)
#endif

#endif /* _SANDIX_LINKAGE_H_ */
