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
 * This file describes TTY Layer Structures in Sandix.
 */

#ifndef _SANDIX_TTY_H_
#define _SANDIX_TTY_H_

#include <sandix/compiler.h>
#include <sandix/console.h>
#include <sandix/types.h>
#include <sandix/termios.h>

struct tty_struct;

/*
 * This structure defines the interface between the low-level tty driver and
 * the tty routines. The following routines can be defined; unless noted
 * otherwise, they are optional, and can be filled in with a null pointer.
 */
struct tty_operations {
	int	(*write)(struct tty_struct *tty, const unsigned char *buf, int count);
	int	(*put_char)(struct tty_struct *tty, unsigned char ch);
	void	(*set_termios)(struct tty_struct *tty, struct termios *old);
};

struct tty_driver {
	const char	*name;			/* Driver Name */
	int		type;			/* Type of TTY driver */
	unsigned int	major;			/* Major Number */
	unsigned int	minor_start;		/* Start of minor device number */
	unsigned int	num;			/* Number of devices allocated */
	struct termios	init_termios;
	
	const struct tty_operations *ops;
	struct list_head tty_drivers;
};

struct tty_struct {
	struct tty_driver	*driver;
	struct vc_struct	*console;
};

/* TTY Driver Types */
#define TTY_DRIVER_TYPE_SYSTEM		0x0001
#define TTY_DRIVER_TYPE_CONSOLE		0x0002
#define TTY_DRIVER_TYPE_SERIAL		0x0003
#define TTY_DRIVER_TYPE_PTY		0x0004
#define TTY_DRIVER_TYPE_SCC		0x0005
#define TTY_DRIVER_TYPE_SYSCONS		0x0006


void tty_set_operations(struct tty_driver *driver, const struct tty_operations *ops);
int tty_unregister_driver(struct tty_driver *driver);
int tty_register_driver(struct tty_driver *driver);

void __init tty_init(void);

#endif /* _SANDIX_TTY_H_ */
