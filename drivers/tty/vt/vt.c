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
 *         -->Virtual Console (VC)
 *               -->Console Driver (VGA, MDA.)
 *
 *   This file is the VT & VC layer.
 *   The console driver layer is in driver/video/xxxcon.c
 *
 * Mon Jul 27 18:10:27 CST 2015
 */

#include <sandix/compiler.h>
#include <sandix/console.h>
#include <sandix/tty.h>
#include <sandix/types.h>

/*
 * this is what the terminal answers to a ESC-Z or csi0c query.
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

static void scrup(struct vc *vc)
{

}

static void scrdown(struct vc *vc)
{

}


/*
 * active vc and his con_driver.
 * For now, only one vc whose driver is vga_con is active. 
 * Since I just want to show the design.
 * But implement with simplicity.
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
	
	/* ONE VGA_CON VC AVALIABLE, OTHERS DUMMY */
	for (i = 0; i < MAX_NR_CONSOLES; i++) {
		vc_bind_driver(&vc_cons[i], &dummy_con);
	}
	vc_bind_driver(&vc_cons[0], &vga_con);
	vc_active = &vc_cons[0];
	con_active = vc_active->driver;
	
}

