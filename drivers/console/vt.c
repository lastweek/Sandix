/*
 *	Basic VT102 implementation.
 *
 *	Copyright (C) 2015 Yizhou Shan
 *
 *	The interface to the hardware is specified using a special structure
 *	(struct con_driver) which contains function pointers to console
 *	operations (see <sandix/console.h> for more information).
 *
 *	The abstract console driver provides a generic interface for a text
 *	console. It supports VGA text mode, MDA text mode, dummy console.
 *
 *	The interface to the tty is specified using a special structure
 *	(struct vc_struct) which contains data and operations for single virtual
 *	console(see <sandix/console.h> for more information).
 *
 *	The different layer can been seen as:
 *	_________________________________________________________________
 *	|-->TTY Layer							|
 *	|	-->Line Discipline					|
 *	|		-->Virtual Terminal (VT)			|
 *	|		-->Virtual Console  (VC)			|
 *	|			-->Console Driver (VGA, MDA, DUMMY)	|	
 *	|_______________________________________________________________|
 */

#include <sandix/compiler.h>
#include <sandix/console.h>
#include <sandix/errno.h>
#include <sandix/tty.h>
#include <sandix/types.h>

/*
 * This is what the terminal answers to a ESC-Z or csi0c query.
 */
#define VT100ID "\033[?1;2c"
#define VT102ID "\033[?6c"

const struct con_driver *registed_con_drivers[MAX_NR_CON_DRIVERS];
struct vc_struct vc_struct_map[MAX_NR_CONSOLES];

EXPORT_SYMBOL(registed_con_drivers);
EXPORT_SYMBOL(vc_struct_map);

/****************************************************************************/
/*			Screen Manipulation				    */
/****************************************************************************/

ALWAYS_INLINE void scrup(struct vc_struct *vc, int lines)
{
	vc->driver->con_scroll(vc, VWIN_UP, lines);
}

ALWAYS_INLINE void scrdown(struct vc_struct *vc, int lines)
{
	vc->driver->con_scroll(vc, VWIN_DOWN, lines);
}

ALWAYS_INLINE void save_cursor(struct vc_struct *vc)
{
	vc->vc_saved_x = vc->vc_x;
	vc->vc_saved_y = vc->vc_y;
}

ALWAYS_INLINE void restore_cursor(struct vc_struct *vc)
{
	vc->vc_x = vc->vc_saved_x;
	vc->vc_y = vc->vc_saved_y;
}

enum {
	VT_NORMAL,
	VT_ESC,
	VT_CSI_S1,
	VT_CSI_S2,
	VT_CSI_S3
};

/**
 * con_write - write to VT screen
 *
 * The data has aleady been cooked by Line Discipline Layer(or not). Here the
 * data string is passed down to console driver layer, who communicate with
 * dedicated hardware diretly.
 *
 * We emulate VT102 by cooking escape and control sequences. Escape and control
 * sequences provide additional control functions not provided by the single
 * character controls of the character set. These multiple-character sequences
 * are not displayed; instead, they control terminal operation.
 */
int con_write(struct tty_struct *tty, const unsigned char *buf, int count)
{
	int c, state;
	struct vc_struct *vc;
	struct con_driver *con;

	vc = tty->console;
	con = tty->console->driver;
	state = VT_NORMAL;
	
	while (count) {
		c = *buf;
		count--;
		buf++;
		
		switch (state) {
		case (VT_NORMAL):
			if (c > 31 && c < 127) {
			
			} else if (c == ESC_ASCII) {
				state = VT_ESC;
			} else if (c == ) {
				
			}
			break;
		case (VT_ESC):
			state = VT_NORMAL;
			if (c == '[')
				state = VT_CSI_S1;
			else if (c == 'D')
				;
			else if (c == 'E')
				;
			break;
		case (VT_CSI_S1):

		};
	}

	return 0;
}


/****************************************************************************/
/*			VT Layer Management				    */
/****************************************************************************/

int bind_con_driver(struct vc_struct *vc, const struct con_driver *con)
{
	int i, err;

	err = -EINVAL;
	for (i = 0; i < MAX_NR_CON_DRIVERS; i++) {
		if (con == registed_con_drivers[i]) {
			err = 0;
			break;
		}
	}
	
	if (!err)
		vc->driver = con;
	
	return err;
}

/*TODO*/
int unbind_con_driver(const struct con_driver *con)
{
	return 0;
}

/**
 * register_con_driver
 * @con - The new console driver to register
 *
 * DESCRIPTION: Register a new console driver. After a success registration,
 * new console's con_startup() will be called to initialize driver itself.
 */
int register_con_driver(const struct con_driver *con)
{
	int i, err;

	err = 0;
	for (i = 0; i < MAX_NR_CON_DRIVERS; i++) {
		if (con == registed_con_drivers[i]) {
			err = -EBUSY;
			break;
		}
	}

	if (err)
		return err;

	err = -EINVAL;
	for (i = 0; i < MAX_NR_CON_DRIVERS; i++) {
		if (registed_con_drivers[i] == NULL) {
			registed_con_drivers[i] = con;
			err = 0;
			break;
		}
	}
	
	con->con_startup();

	return err;
}

/**
 * unregister_con_driver
 * @con: The old console to unregister.
 *
 * DESCRIPTION: Before unregister a console driver, the driver needs to be
 * unbinded from any vc_struct. An active vc_struct is not allowed to unbind
 * its console driver, in that case, unregistration fails.
 */
int unregister_con_driver(const struct con_driver *con)
{
	int i, err;

	err = unbind_con_driver(con);
	if (err)
		return err;

	err = -EINVAL;
	for (i = 0; i < MAX_NR_CON_DRIVERS; i++) {
		if (con == registed_con_drivers[i]) {
			registed_con_drivers[i] = NULL;
			err = 0;
			break;
		}
	}

	return err;
}

/*
 * Register console drivers, and then initialize every virtual console.
 * That is all this function responsible for, actually. Also you will see
 * that only the first vc is bind to vga_con, and yes, that is the only
 * console sandix use.
 */
void __init con_init(void)
{
	int i;
	
	register_con_driver(&dummy_con);
	register_con_driver(&vga_con);

	for (i = 1; i < MAX_NR_CONSOLES; i++) {
		bind_con_driver(&vc_struct_map[i], &dummy_con);
	}

	bind_con_driver(&vc_struct_map[0], &vga_con);
	vc_struct_map[0].driver->con_init(&vc_struct_map[0]);
	
	/* Now it is safe to use FG_VC */
}
