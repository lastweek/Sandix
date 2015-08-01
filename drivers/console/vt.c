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
#include <video/video.h>

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

ALWAYS_INLINE
void scrup(struct vc_struct *vc, int lines)
{
	vc->driver->con_scroll(vc, VWIN_UP, lines);
}

ALWAYS_INLINE
void scrdown(struct vc_struct *vc, int lines)
{
	vc->driver->con_scroll(vc, VWIN_DOWN, lines);
}

ALWAYS_INLINE
void save_cursor(struct vc_struct *vc)
{
	vc->vc_saved_x = vc->vc_x;
	vc->vc_saved_y = vc->vc_y;
}

ALWAYS_INLINE
void restore_cursor(struct vc_struct *vc)
{
	vc->vc_x = vc->vc_saved_x;
	vc->vc_y = vc->vc_saved_y;
}

ALWAYS_INLINE
void hide_cursor(struct vc_struct *vc)
{
	vc->driver->con_cursor(vc, CM_ERASE);
}

ALWAYS_INLINE
void set_cursor(struct vc_struct *vc)
{
	vc->driver->con_cursor(vc, CM_DRAW);
}

ALWAYS_INLINE
void carriage_return(struct vc_struct *vc)
{
	vc->vc_pos -= (vc->vc_x << 1);
	vc->vc_x = 0;
}

static void line_feed(struct vc_struct *vc)
{
	if ((vc->vc_y + 1) == vc->vc_rows)
		scrdown(vc, 1);
	else
		vc->vc_y++;
	
	vc->vc_pos += vc->vc_row_size;
}

static void reverse_line_feed(struct vc_struct *vc)
{
	
}

static void gotoxy(struct vc_struct *vc, int new_x, int new_y)
{
	if (new_x >= vc->vc_cols || new_y >= vc->vc_rows)
		return;
	vc->vc_x = new_x;
	vc->vc_y = new_y;
	vc->vc_pos = vc->vc_visible_origin +
		(((vc->vc_cols*vc->vc_y) + vc->vc_x) << 1);
}

static void respond(struct tty_struct *tty)
{

}

/* Erase Display */
static void csi_J(struct vc_struct *vc)
{
	unsigned int count;
	unsigned short *start;

	switch (vc->vc_par[0]) {
		case 0:	/* Erase from cursor to end of display */
			count = (vc->vc_scr_end - vc->vc_pos) >> 1;
			start = (unsigned short *)vc->vc_pos;
			break;
		case 1:	/* Erase from start to cursor */
			count = ((vc->vc_pos - vc->vc_visible_origin) >> 1) + 1;
			start = (unsigned short *)vc->vc_visible_origin;
			break;
		case 2:	/* Erase whole display */
			count = vc->vc_cols * vc->vc_rows;
			start = (unsigned short *) vc->vc_visible_origin;
			break;
		case 3:	/* Erase scroll-back buffer */
			count = ((vc->vc_pos - vc->vc_origin) >> 1) + 1;
			start = (unsigned short *) vc->vc_origin;
			break;
		default:
			return;
	};
	
	scr_memsetw(start, vc->vc_erased_char, count);
}

/* Erase Line */
static void csi_K(struct vc_struct *vc)
{
	
}

static void csi_L(struct vc_struct *vc)
{

}

static void csi_M(struct vc_struct *vc)
{

}

static void csi_P(struct vc_struct *vc)
{

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

#define BS	8		/* Back Space */
#define HT	9		/* Horizontal Table */
#define NL	10		/* New Line */
#define VT	11		/* Vertical Tab */
#define NP	12		/* New Page */
#define CR	13		/* Carriage Return */
#define ESC	27		/* Escape */
#define DEL	127		/* Delete */

	int npar, c, state;
	struct vc_struct *vc;
	static struct con_driver *con;

	vc = tty->console;
	con = tty->console->driver;
	state = VT_NORMAL;
	
	hide_cursor(vc);

	while (count) {
		c = *buf;
		count--;
		buf++;
		
		switch (state) {
		case (VT_NORMAL):
			if (c > 31 && c < 127) {
				if (vc->vc_x == vc->vc_cols) {
					vc->vc_x = 0;
					vc->vc_pos -= vc->vc_row_size;
					line_feed(vc);
				}
				con->con_putc(vc, c, vc->vc_y, vc->vc_x);
				vc->vc_x++;
				vc->vc_pos += 2;
			} else if (c == BS) {
				if (vc->vc_x) {
					vc->vc_x--;
					vc->vc_pos -= 2;
				}
			} else if (c == HT) {
				/* Table Width = 8 */
				c = 8 - (vc->vc_x & 7);
				vc->vc_x += c;
				vc->vc_pos += c << 1;
				if (vc->vc_x >= vc->vc_cols) {
					vc->vc_x -= vc->vc_cols;
					vc->vc_pos -= vc->vc_row_size;
					line_feed(vc);
				}
			} else if (c == NL || c == VT || c == NP) {
				line_feed(vc);
			} else if (c == CR) {
				carriage_return(vc);
			} else if (c == ESC) {
				state = VT_ESC;
			}
			break;
		/* ESC- but not CSI- */
		case (VT_ESC):
			state = VT_NORMAL;
			if (c == '[') {
				state = VT_CSI_S1;
			} else if (c == 'D') {
				line_feed(vc);
			} else if (c == 'E') {
				gotoxy(vc, 0, vc->vc_y++);
			} else if (c == 'M') {
				reverse_line_feed(vc);
			} else if (c == 'Z') {
				respond(tty);
			} else if (c == '7') {
				save_cursor(vc);
			} else if (c == '8') {
				restore_cursor(vc);
			}
			break;
		case (VT_CSI_S1):
			state = VT_CSI_S2;
			npar = 0;
			if (c == '?') {
				/* \033[? */
				break;
			}
		case (VT_CSI_S2):
			if (c == ';' && npar < (NPAR - 1)) {
				npar++;
				break;
			} else if (c >= '0' && c <= '9') {
				vc->vc_par[npar] = 10 * vc->vc_par[npar]
						    + c - '0';
				break;
			} else {
				state = VT_CSI_S3;
			}
		case (VT_CSI_S3):
			state = VT_NORMAL;
			switch (c) {
				case 'G': case '`':
					if (vc->vc_par[0])
						vc->vc_par[0]--;
					gotoxy(vc, vc->vc_par[0], vc->vc_y);
					break;
				case 'A':
					if (!vc->vc_par[0])
						vc->vc_par[0]++;
					gotoxy(vc, vc->vc_x, vc->vc_y - vc->vc_par[0]);
					break;
				case 'B': case 'e':
					if (!vc->vc_par[0])
						vc->vc_par[0]++;
					gotoxy(vc, vc->vc_x, vc->vc_y + vc->vc_par[0]);
					break;
				case 'C': case 'a':
					if (!vc->vc_par[0])
						vc->vc_par[0]++;
					gotoxy(vc, vc->vc_x + vc->vc_par[0], vc->vc_y);
					break;
				case 'D':
					if (!vc->vc_par[0])
						vc->vc_par[0]++;
					gotoxy(vc, vc->vc_x + vc->vc_par[0], vc->vc_y);
					break;
				case 'E': 
					if (!vc->vc_par[0])
						vc->vc_par[0]++;
					gotoxy(vc, 0, vc->vc_y + vc->vc_par[0]);
					break;
				case 'F':
					if (!vc->vc_par[0])
						vc->vc_par[0]++;
					gotoxy(vc, 0, vc->vc_y - vc->vc_par[0]);
					break;
				case 'd':
					if (vc->vc_par[0])
						vc->vc_par[0]--;
					gotoxy(vc, vc->vc_x, vc->vc_par[0]);
					break;
				case 'H': case 'f':
					if (vc->vc_par[0])
						vc->vc_par[0]--;
					if (vc->vc_par[1])
						vc->vc_par[1]--;
					gotoxy(vc, vc->vc_par[1], vc->vc_par[0]);
					break;
				case 'J':
					csi_J(vc);
					break;
				case 'K':
					csi_K(vc);
					break;
				case 'L':
					csi_L(vc);
					break;
				case 'M':
					csi_M(vc);
					break;
				case 'P':
					csi_P(vc);
					break;
				case 's':
					save_cursor(vc);
					break;
				case 'u':
					restore_cursor(vc);
					break;

			};
		default:
			return 0;
		}; /* End of switch(state) */
	} /* End of while() */
	
	set_cursor(vc);

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
