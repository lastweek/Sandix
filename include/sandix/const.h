#ifndef _SANDIX_CONST_H_
#define _SANDIX_CONST_H_

/*
 * const.h: Macros for dealing with constants.
 *
 * Some constant macros are used in both assembler and
 * C code. Therefore we cannot annotate them always with
 * 'UL' and other type specifiers unilaterally.  We
 * use the following macros to deal with this.
 *
 * Similarly, _AT() will cast an expression with a type in C, but
 * leave it unchanged in asm.
 */
 
#ifdef __ASSEMBLY__
#define _AC(X,Y)    X
#define _AT(T,X)    X
#else
#define _AC(X,Y)	(X##Y)
#define _AT(T,X)	((T)(X))
#endif

#define _BITUL(x)	(_AC(1,UL) << (x))
#define _BITULL(x)	(_AC(1,ULL) << (x))

#endif /* _SANDIX_CONST_H_ */
