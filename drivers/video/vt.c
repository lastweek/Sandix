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
 *   (struct vc) which contains data and operations for a single virtual
 *   console.(see <sandix/console.h> for more information)
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
 *   The console driver layer is in driver/video/xxxcon.c
 *
 * Mon Jul 27 18:10:27 CST 2015
 */

#include <sandix/compiler.h>
#include <sandix/console.h>
#include <sandix/errno.h>
#include <sandix/tty.h>
#include <sandix/types.h>

/*
 * System registed console driver, and virtual console maps.
 */
static struct con_driver *registed_con_drivers[MAX_NR_CON_DRIVERS];
static struct vc_struct vc_struct_map[MAX_NR_CONSOLES];
static struct vc_struct *vc_active;
static struct con_driver *con_active;

/*
 * This is what the terminal answers to a ESC-Z or csi0c query.
 */
#define VT100ID "\033[?1;2c"
#define VT102ID "\033[?6c

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
	
}

static int con_write(struct tty_struct *tty, const unsigned char *buf, int count)
{
	return do_con_write(tty, buf, count);
}

static int con_put_char(struct tty_struct *tty, unsigned char ch)
{
	return do_con_write(tty, &ch, 1);
}

int bind_con_driver(struct vc_struct *vc, struct con_driver *con)
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

int unbind_con_driver(struct con_driver *con)
{
	return 0;
}

/**
 * register_con_driver
 * @newcon - The new console driver to register
 *
 * DESCRIPTION: Register a new console driver. After a success registration,
 * new console's con_startup() will be called to initialize driver itself.
 */
int register_con_driver(struct con_driver *newcon)
{
	int i, err;

	err = 0;
	for (i = 0; i < MAX_NR_CON_DRIVERS; i++) {
		if (newcon == registed_con_drivers[i]) {
			err = -EBUSY;
			break;
		}
	}

	if (err)
		return err;

	err = -EINVAL;
	for (i = 0; i < MAX_NR_CON_DRIVERS; i++) {
		if (registed_con_drivers[i] == NULL) {
			registed_con_drivers[i] = newcon;
			newcon->con_startup();
			err = 0;
			break;
		}
	}

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
int unregister_con_driver(struct con_driver *con)
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

void __init con_init(void)
{
	int i;
	
	/* ONE VGA_CON VC AVALIABLE, OTHERS DUMMY */
	for (i = 0; i < MAX_NR_CONSOLES; i++) {
		bind_con_driver(&vc_cons[i], &dummy_con);
	}
	vc_bind_driver(&vc_cons[0], &vga_con);
	vc_active = &vc_cons[0];
	con_active = vc_active->driver;
	
}
