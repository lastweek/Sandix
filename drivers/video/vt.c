/*
 * Hopefully this will be a rather _basic_ VT102 implementation.
 * It normally means that the VT will respond to escape sequences
 * in the same way as a real VT102 terminal.
 *
 * Note:
 *
 *   The interface to the hardware is specified using a special structure
 *   (struct con_driver) which contains function pointers to console operations
 *   (see <sandix/console.h> for more information).
 *
 *   The abstract console driver provides a generic interface for a text
 *   console. It supports VGA text mode, MDA text mode, dummy console.
 *
 *   The interface to the tty is specified using a special structure
 *   (struct vc_struct) which contains data and operations for single virtual
 *   console(see <sandix/console.h> for more information).
 *
 *   The different layer can been seen as:
 *   
 *   -->TTY Layer
 *      -->Line Discipline
 *         -->Virtual Terminal (VT)
 *         -->Virtual Console  (VC)
 *               -->Console Driver (VGA, MDA.)
 *
 *   This file is the VT & VC layer.
 *   Specific console drivers are in driver/video/xxxcon.c
 *
 * Mon Jul 27 18:10:27 CST 2015
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

/*
 * System registed console driver, and virtual console maps.
 */
struct con_driver *registed_con_drivers[MAX_NR_CON_DRIVERS];
struct vc_struct vc_struct_map[MAX_NR_CONSOLES];

/**
 * do_con_write - write to VT screen
 * 
 * DESCRIPTION:
 * Escape and control sequences provide additional control functions not
 * provided by the single-character controls of the character set. These
 * multiple-character sequences are not displayed; instead, they control
 * terminal operation.
 */
static int do_con_write(struct tty_struct *tty, const unsigned char *buf, int count)
{
	return 0;
}

static int con_write(struct tty_struct *tty, const unsigned char *buf, int count)
{
	return do_con_write(tty, buf, count);
}

static int con_put_char(struct tty_struct *tty, unsigned char ch)
{
	return do_con_write(tty, &ch, 1);
}

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
	
	/* Now it is safe to use ACTIVE_VC */
}
