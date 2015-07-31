#ifndef _SANDIX_TTY_H_
#define _SANDIX_TTY_H_

#include <sandix/console.h>

/**
 *	struct tty_struct
 *	@console:	The console this tty connect to.
 *
 */
struct tty_struct {
	struct vc_struct	*console;
};

#endif /* _SANDIX_TTY_H_ */
