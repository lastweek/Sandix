#ifndef _SANDIX_CONSOLE_H_
#define _SANDIX_CONSOLE_H_

#include <sandix/types.h>
#include <sandix/tty.h>

#define NPAR			16

struct con_driver;

/*
 * Data structure describing a single virtual console.
 * Various VCs can have the same CONSW.
 */
struct vc {
	struct con_driver	*driver;			/* Low-Level driver */
	unsigned int	vc_num;			/* Console number */
	unsigned int	vc_rows;
	unsigned int	vc_cols;
	unsigned int	vc_row_size;		/* Bytes per row */
	unsigned int	vc_scan_line;		/* Cursor scanlines? */
	unsigned int	vc_origin;		/* Start of real screen */
	unsigned int	vc_scr_end;		/* End of real screen */
	unsigned int	vc_visible_origin;	/* Top of visible window */
	unsigned int	vc_top, vc_bottom;	/* Scrolling region */
	
	/* attributes for characters */
	unsigned int	vc_attr;		/* Current attributes */
	unsigned int	vc_f_color;		/* Foreground color */
	unsigned int	vc_b_color;		/* Background color */

	/* cursor */
	unsigned int	vc_x;
	unsigned int	vc_y;
	unsigned int	vc_saved_x;
	unsigned int	vc_saved_y;
	unsigned int	vc_pos;			/* Cursor position address */

	/* VT terminal data */
	unsigned int	vc_state;		/* Escape sequence parser state */
	unsigned int	vc_npar;
	unsigned int	vc_par[NPAR];		/* Parameter of current escape sequence */
	
};

/*
 * All avaliable virtual consoles in system.
 * Defined in drivers/tty/vt/vt.c
 */
extern struct vc vc_cons[MAX_NR_CONSOLES];

/*
 * Low-level console driver.
 * We call it console driver because it has
 * operations that can talk to hardware directly.
 */
struct con_driver {
	const char *(*con_startup)(void);
	void	(*con_init)(struct vc *, int);
	void	(*con_deinit)(struct vc *);
	void	(*con_clear)(struct vc *, int, int, int, int);
	void	(*con_putc)(struct vc *, int, int, int);
	void	(*con_putcs)(struct vc *, unsigned short *, int, int, int);
	void	(*con_cursor)(struct vc *);
	int	(*con_scroll)(struct vc *, int, int, int, int);
	void	(*con_bmove)(struct vc *, int, int, int, int, int, int);
	int	(*con_switch)(struct vc *);
	int	(*con_blank)(struct vc *, int, int);
	int     (*con_resize)(struct vc *, unsigned int, unsigned int,
			       unsigned int);
	int	(*con_scrolldelta)(struct vc *, int);
	int	(*con_set_origin)(struct vc *);
	void	(*con_save_screen)(struct vc *);
	void	(*con_invert_region)(struct vc *, u16 *, int);
	u16    *(*con_screen_pos)(struct vc *, int);
	u32	(*con_getxy)(struct vc *, unsigned long, int *, int *);
};

/*
 * Three CONSWs are avaliable in Sandix.
 * Only one of them is usable, vga_con.
 */
extern const struct con_driver dummy_con;
extern const struct con_driver vga_con;
extern const struct con_driver mda_con;

struct console {
	char	name[16];
	void	(*write)(struct console *, const char *, unsigned);
	int	(*read)(struct console *, char *, unsigned);
	struct	tty_driver *(*device)(struct console *, int *);
	void	(*unblank)(void);
	int	(*setup)(struct console *, char *);
	int	(*match)(struct console *, char *name, int idx, char *options);
	short	flags;
	short	index;
	int	cflag;
	void	*data;
	struct	console *next;
};

#endif /* _SANDIX_CONSOLE_H_ */
