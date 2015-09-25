/*
 *	Basic VT102 Terminal Simulation.
 *
 *	Copyright (C) 2015 Yizhou Shan <shanyizhou@ict.ac.cn>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License along
 *	with this program; if not, write to the Free Software Foundation, Inc.,
 *	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/*
 *	The interface to the hardware is specified using a special structure
 *	(struct con_driver) which contains function pointers to console
 *	operations (see <sandix/console.h> for more information). The abstract
 *	console driver provides a generic interface for a text console. It
 *	supports VGA text mode, MDA text mode, dummy console. (For simplicity,
 *	the virtual console layer is NOT well ported to low-level drivers.)
 *
 *	The interface to the TTY layer is specified using a special structure
 *	(struct tty_driver) which contains function pointers to virtual console
 *	manipulations that described in this file.
 *
 *	The abstraction of these layers:
 *
 *	-->TTY Layer
 *		-->Line Discipline(Cook)
 *			-->Virtual Console  (VC)
 *				-->Console Driver (VGA, MDA, DUMMY)
 */

#include <sandix/compiler.h>
#include <sandix/console.h>
#include <sandix/errno.h>
#include <sandix/export.h>
#include <sandix/types.h>
#include <sandix/major.h>
#include <sandix/magic.h>
#include <sandix/tty.h>
#include <sandix/bug.h>

#include <video/video.h>

/* Answers to a ESC-Z or CSI0c query. */
#define VT100ID "\033[?1;2c"
#define VT102ID "\033[?6c"

/* Registed console drivers. e.g. VGA console driver */
const struct con_driver *registed_con_drivers[MAX_NR_CON_DRIVERS];
EXPORT_SYMBOL(registed_con_drivers);

/* Virtual Console MAP in Sandix. */
struct vc_struct vc_struct_map[MAX_NR_CONSOLES];
EXPORT_SYMBOL(vc_struct_map);


/****************************************************************************/
/*			Basic Screen Manipulation			    */
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

ALWAYS_INLINE void hide_cursor(struct vc_struct *vc)
{
	vc->driver->con_cursor(vc, CM_ERASE);
}

ALWAYS_INLINE void set_cursor(struct vc_struct *vc)
{
	vc->driver->con_cursor(vc, CM_DRAW);
}

ALWAYS_INLINE void carriage_return(struct vc_struct *vc)
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

__MUST_TODO__
__unused static void respond(struct tty_struct *tty)
{
	
}

static void respond_ID(struct tty_struct *tty)
{

}

static void delete_char(struct vc_struct *vc, unsigned int nr)
{
	unsigned short *p = (unsigned short *)vc->vc_pos;

	scr_memcpyw(p, p + nr, vc->vc_cols - vc->vc_x - nr);
	scr_memsetw(p + vc->vc_cols - vc->vc_x - nr, vc->vc_erased_char, nr);
}

static void insert_char(struct vc_struct *vc,  unsigned int nr)
{
	unsigned short *p = (unsigned short *)vc->vc_pos;

	scr_memmovew(p + nr, p, vc->vc_cols - vc->vc_x - nr);
	scr_memsetw(p, vc->vc_erased_char, nr);
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
	unsigned int count;
	unsigned short *start;

	switch (vc->vc_par[0]) {
		case 0:	/* Erase from cursor to end of line */
			count = vc->vc_cols - vc->vc_x;
			start = (unsigned short *)vc->vc_pos;
			break;
		case 1: /* Erase from start of line to cursor */
			count = vc->vc_x + 1;
			start = (unsigned short *)(vc->vc_pos - (vc->vc_x << 1));
		case 2: /* Erase whole line */
			count = vc->vc_cols;
			start = (unsigned short *)(vc->vc_pos - (vc->vc_x << 1));
		default:
			return;
	};

	scr_memsetw(start, vc->vc_erased_char, count);
}

/* Insert Lines */
static void csi_L(struct vc_struct *vc)
{
	unsigned int lines = vc->vc_par[0];

	if (lines > (vc->vc_rows - vc->vc_y)) {
		lines = vc->vc_rows - vc->vc_y;
	} else if (!lines) {
		lines = 1;
	}
}

/* Delete Lines */
static void csi_M(struct vc_struct *vc)
{
	unsigned int lines = vc->vc_par[0];

	if (lines > (vc->vc_rows - vc->vc_y)) {
		lines = vc->vc_rows - vc->vc_y;
	} else if (!lines) {
		lines = 1;
	}
}

/* Delete chars in current line */
static void csi_P(struct vc_struct *vc)
{
	unsigned int nr = vc->vc_par[0];

	if (nr > (vc->vc_cols - vc->vc_x)) {
		nr = vc->vc_cols - vc->vc_x;
	} else if (!nr) {
		nr = 1;
	}
	delete_char(vc, nr);
}

/* Erase chars in current line */
static void csi_X(struct vc_struct *vc)
{
	unsigned int nr = vc->vc_par[0];

	if (!nr)
		nr++;
	nr = (nr > vc->vc_cols - vc->vc_x) ? (vc->vc_cols - vc->vc_x) : nr;
	scr_memsetw((unsigned short *)vc->vc_pos, vc->vc_erased_char, nr);
}

static void update_attribute(struct vc_struct *vc)
{
	vc->vc_attr = (vc->vc_f_color & 0x7)	|
		((vc->vc_b_color & 0x7)	<< 4)	|
		((vc->vc_blink & 0x1) << 7)	;
	
	vc->vc_erased_char &= 0xff;
	vc->vc_erased_char |= vc->vc_attr;
}

/* Set attribute */
static void csi_m(struct vc_struct *vc)
{
	int i;

	for (i = 0; i <= vc->vc_npar; i++) {
		switch (vc->vc_par[i]) {
			case 0:	/* Default */
				vc->vc_bold = 0;
				vc->vc_underline = 0;
				vc->vc_blink = 0;
				vc->vc_f_color = 7;
				vc->vc_b_color = 0;
				break;
			case 1:	/* Bold */
				vc->vc_bold = 1;
				break;
			case 4:	/* Underline */
				vc->vc_underline = 1;
				break;
			case 5: /* Blink */
				vc->vc_blink = 1;
				break;
			case 30:case 31:case 32:case 33:
			case 34:case 35:case 36:case 37:
				vc->vc_f_color = vc->vc_par[i] - 30;
				break;
			case 40:case 41:case 42:case 43:
			case 44:case 45:case 46:case 47:
				vc->vc_b_color = vc->vc_par[i] - 40;
				break;
			case 38:
				vc->vc_f_color = 7;
				break;
			case 48:
				vc->vc_b_color = 0;
				break;
			default:
				break;
		}
	}
	update_attribute(vc);
}

/* Insert blank chars in current line */
static void csi_at(struct vc_struct *vc)
{
	unsigned int nr = vc->vc_par[0];

	if (nr > vc->vc_cols - vc->vc_x) {
		nr = vc->vc_cols - vc->vc_x;
	} else if (!nr) {
		nr = 1;
	}
	insert_char(vc, nr);
}

enum {
	VT_NORMAL,
	VT_ESC,
	VT_CSI_QUESTION,
	VT_CSI_PARAMETER,
	VT_CSI_HANDLE
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
	const struct con_driver *con;

	vc = (struct vc_struct *)tty->driver_data;
	con = vc->driver;
	state = VT_NORMAL;
	npar = 0;

	WARN_ON(vc->magic != TTY_CONSOLE_DATA);
	
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
				state = VT_CSI_QUESTION;
			} else if (c == 'D') {
				line_feed(vc);
			} else if (c == 'E') {
				gotoxy(vc, 0, vc->vc_y++);
			} else if (c == 'M') {
				reverse_line_feed(vc);
			} else if (c == 'Z') {
				respond_ID(tty);
			} else if (c == '7') {
				save_cursor(vc);
			} else if (c == '8') {
				restore_cursor(vc);
			}
			break;
		case (VT_CSI_QUESTION):
			for (npar = 0; npar < NPAR; npar++)
				vc->vc_par[npar] = 0;
			npar = 0;
			state = VT_CSI_PARAMETER;
			if (c == '?') {
				/* \033[? */
				break;
			}
		case (VT_CSI_PARAMETER):
			if (c == ';' && npar < (NPAR - 1)) {
				npar++;
				break;
			} else if (c >= '0' && c <= '9') {
				vc->vc_par[npar] = 10 * vc->vc_par[npar]
						+ (c - '0');
				break;
			} else {
				state = VT_CSI_HANDLE;
				vc->vc_npar = npar;
			}
		case (VT_CSI_HANDLE):
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
				case 'X':
					csi_X(vc);
					break;
				case 'c':
					if (!vc->vc_par[0])
						respond_ID(tty);
					break;
				case 'm':
					csi_m(vc);
					break;
				case 'r':
					if (!vc->vc_par[0])
						vc->vc_par[0]++;
					if (!vc->vc_par[1])
						vc->vc_par[1] = vc->vc_rows;
					/* Minimum allowed region is 2 lines */
					if (vc->vc_par[0] < vc->vc_par[1] &&
					    vc->vc_par[1] <= vc->vc_rows) {
						vc->vc_top = vc->vc_par[0] - 1;
						vc->vc_bottom = vc->vc_par[1];
						/* Move to home position */
						gotoxy(vc, 0, 0);
					}
					break;
				case 's':
					save_cursor(vc);
					break;
				case 'u':
					restore_cursor(vc);
					break;
				case '@':
					csi_at(vc);
					break;
			};
			break;
		}; /* End of switch(state) */
	} /* End of while(count) */
	
	set_cursor(vc);
	return 0;
#undef BS
#undef HT
#undef NL
#undef VT
#undef NP
#undef CR
#undef ESC
#undef DEL
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
EXPORT_SYMBOL(bind_con_driver);

int unbind_con_driver(const struct con_driver *con)
{
	return 0;
}
EXPORT_SYMBOL(unbind_con_driver);

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
	
	/* Succeed, invoke driver startup function */
	if (!err)
		con->con_startup();

	return err;
}
EXPORT_SYMBOL(register_con_driver);

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
EXPORT_SYMBOL(unregister_con_driver);

static const struct tty_operations console_ops = {
	.write		= con_write,
	.put_char	= NULL,
	.set_termios	= NULL
};

struct tty_driver console_driver __read_mostly;

/**
 * console_init
 *
 * Register Low-Level Dummy and VGA Console Drivers.
 * Register TTY Driver for Console.
 */
void __init console_init(void)
{
	int i;
	
	/* Register Dummy Console */
	register_con_driver(&dummy_con);
	for (i = 1; i < MAX_NR_CONSOLES; i++) {
		bind_con_driver(&vc_struct_map[i], &dummy_con);
	}

	/* Register VGA Console, then activate it */
	register_con_driver(&vga_con);
	bind_con_driver(&vc_struct_map[0], &vga_con);
	vc_struct_map[0].driver->con_init(&vc_struct_map[0]);

	/* Initialize TTY driver, then register it */
	console_driver.name = "TTY Driver for Console";
	console_driver.type = TTY_DRIVER_TYPE_CONSOLE;
	console_driver.major = TTY_MAJOR;
	console_driver.minor_start = 1;
	console_driver.num = 1;
	//console_driver.init_termios = ;
	tty_set_operations(&console_driver, &console_ops);
	tty_register_driver(&console_driver);
}
