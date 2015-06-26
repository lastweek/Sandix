#ifndef _SANDIX_CONST_H
#define _SANDIX_CONST_H

/*
 * Macros for dealing with constants.
 */

/* Y is suffix of const type */
#define __AC(X,Y)	(X##Y)
#define _AC(X,Y)	__AC(X,Y)
#define _AT(T,X)	((T)(X))

#define _BITUL(x)	(_AC(1,UL) << (x))
#define _BITULL(x)	(_AC(1,ULL) << (x))

#endif /* _SANDIX_CONST_H */
