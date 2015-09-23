/*
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
 * This file describes Virtual Console used in Sandix.
 *
 * struct vc_struct:  Describe a standalone virtual console.
 * struct con_driver: Describe a console driver for low-level output.
 */

#ifndef _SANDIX_CONSOLE_H_
#define _SANDIX_CONSOLE_H_

#include <sandix/types.h>
#include <sandix/tty.h>

#define NPAR 16

#define MAX_NR_CON_DRIVERS	4
#define MIN_NR_CONSOLES		1
#define MAX_NR_CONSOLES		8

struct con_driver;
struct tty_struct;

/* Data structure describing a single virtual console. */
struct vc_struct {
	const struct con_driver *driver;	/* Low-Level driver */
	unsigned int	vc_num;			/* Console number */
	unsigned int	vc_cols;
	unsigned int	vc_rows;
	unsigned int	vc_row_size;		/* Bytes per row */
	unsigned int	vc_scan_line;		/* Cursor scanlines? */
	unsigned long	vc_origin;		/* Start of real screen */
	unsigned long	vc_scr_end;		/* End of real screen */
	unsigned long	vc_visible_origin;	/* Top of visible window */
	unsigned int	vc_top, vc_bottom;	/* Scrolling region */

	/* Font */
	unsigned short	vc_erased_char;		/* Erase video */
	
	/* Attributes for characters */
	unsigned int	vc_attr;		/* Current attributes */
	unsigned int	vc_blink;		/* Blink Character */
	unsigned int	vc_f_color;		/* Foreground color */
	unsigned int	vc_b_color;		/* Background color */
	unsigned int	vc_italic;		/* Italic characters */
	unsigned int	vc_bold;		/* Bold characters */
	unsigned int	vc_underline;		/* Underline characters */

	/* Cursor */
	unsigned int	vc_x;
	unsigned int	vc_y;
	unsigned int	vc_saved_x;
	unsigned int	vc_saved_y;
	unsigned long	vc_pos;			/* Cursor position address */

	/* VT terminal data */
	unsigned int	vc_state;		/* Escape sequence parser state */
	unsigned int	vc_npar;
	unsigned int	vc_par[NPAR];		/* Parameter of current escape sequence */
	
};

/* Low-Level Console Drivers */
struct con_driver {
	void	(*con_startup)(void);
	void	(*con_init)(struct vc_struct * vc);
	void	(*con_deinit)(struct vc_struct *);
	void	(*con_clear)(struct vc_struct *, int, int, int, int);
	void	(*con_putc)(struct vc_struct *, int, int, int);
	void	(*con_putcs)(struct vc_struct *, const unsigned char *, int, int, int);
	void	(*con_cursor)(struct vc_struct *, int);
	void	(*con_scroll)(struct vc_struct *, int, int);
	void	(*con_set_color)(struct vc_struct *, int, int, int);
	void	(*con_set_origin)(struct vc_struct *);
	void	(*con_save_screen)(struct vc_struct *);
	u16    *(*con_screen_pos)(struct vc_struct *, int);
	u32	(*con_getxy)(struct vc_struct *, unsigned long, int *, int *);
};

/* Avaliable Console Drivers in Sandix */
extern const struct con_driver vga_con;
extern const struct con_driver mda_con;
extern const struct con_driver dummy_con;

/* Registed console drivers and virtual console map in Sandix */
extern const struct con_driver *registed_con_drivers[MAX_NR_CON_DRIVERS];
extern struct vc_struct vc_struct_map[MAX_NR_CONSOLES];

/* Visible WINdow Direction */
#define VWIN_UP		(1)
#define VWIN_DOWN	(2)

/* Cursor Manner */
#define CM_DRAW		(1)
#define CM_ERASE	(2)
#define CM_MOVE		(3)

void __init console_init(void);
int register_con_driver(const struct con_driver *con);
int unregister_con_driver(const struct con_driver *con);
int bind_con_driver(struct vc_struct *vc, const struct con_driver *con);
int unbind_con_driver(const struct con_driver *con);

/* FIXME remove */
int con_write(struct tty_struct *tty, const unsigned char *buf, int count);

/* UGLY FIXME
 * ForeGround Virtual Console
 * ForeGround Virtual Console Driver
 */
#define FG_VC	(&vc_struct_map[0])
#define FG_CON	(FG_VC->driver)

#endif /* _SANDIX_CONSOLE_H_ */
