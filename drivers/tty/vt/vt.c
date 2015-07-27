/*
 * Hopefully this will be a rather complete VT102 implementation.
 * 
 *   The interface to the hardware is specified using a special structure
 *   (struct con_driver) which contains function pointers to console operations
 *   (see <sandix/console.h> for more information).
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
 *         -->Virtual Console (VC)
 *               -->Console Driver (VGA, MDA.)
 *
 *   This file is the VT & VC layer.
 *   The Console Driver layer is in driver/video/xxxcon.c
 */

#include <sandix/compiler.h>
#include <sandix/console.h>
#include <sandix/tty.h>
#include <sandix/types.h>

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

static void scrup(struct vc *vc)
{

}

static void scrdown(struct vc *vc)
{

}




#define MIN_NR_CONSOLES	1
#define MAX_NR_CONSOLES	7

/*
 * ACTIVE vc and driver.
 * For now, only one vc whose driver is vga_con
 * is active. 
 */
static struct vc vc_cons[MAX_NR_CONSOLES];
struct vc *vc_active;
struct con_driver *con_active;

static void vc_bind_driver(struct vc *vc, struct con_driver *con)
{
	vc->driver = con;
}

void __init con_init(void)
{
	int i;
	
	/* ONE VGA_CON IS ENOUGH NOW */
	for (i = 0; i < MAX_NR_CONSOLES; i++) {
		vc_bind_driver(&vc_cons[i], &dummy_con);
	}
	vc_bind_driver(&vc_cons[0], &vga_con);
	vc_active = &vc_cons[0];
	con_active = vc_active->driver;
	
}

