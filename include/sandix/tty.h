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

/* struct tty_operations
 * @write:		Write to tty device
 * @put_char:		Write one character to tty device
 *
 * This structure defines the interface between the low-level tty driver and
 * the tty routines. The following routines can be defined; unless noted
 * otherwise, they are optional, and can be filled in with a null pointer.
 */
struct tty_operations {
	int	(*write)(struct tty_struct *tty, const unsigned char *buf, int count);
	int	(*put_char)(struct tty_struct *tty, unsigned char ch);
};

/**
 * struct tty_driver
 * @name:		Nick name
 * @type:		Type of this tty driver
 * @major:		Major number of this tty driver
 * @minor_start:	Starting minor number of this tty driver
 * @num:		Number of devices allocated
 * @ops:		Operations of this tty driver
 * @tty_drivers:	Linked list of all registed tty driver
 */
struct tty_driver {
	const char	*name;		
	unsigned int	type;		
	unsigned int	major;		
	unsigned int	minor_start;	
	unsigned int	num;		
	const struct tty_operations *ops;
	struct list_head tty_drivers;
};

/**
 * struct tty_ldisc
 * @read:		Read method
 * @write:		Write method
 * @set_termios:	Replace termios
 * @tty:		tty_struct who own this line discipline
 */
struct tty_ldisc {
	int	(*read)(struct tty_struct *tty, unsigned char __user *buf, size_t len);
	int	(*write)(struct tty_struct *tty, const unsigned char *buf, size_t len);
	void	(*set_termios)(struct tty_struct *tty, struct termios *old);
	struct tty_struct *tty;
};

/**
 * struct tty_struct
 * @magic:		Magic number for this structure
 * @termios:		Termios for this tty device
 * @ldisc:		Line discipline for this tty device
 * @driver:		Low-Level tty driver for this tty device
 * @ops:		Operations of low-level tty driver
 * @disc_data:		Additional data used by line discipline
 * @driver_data:	Additional data used by low-level tty driver
 */
struct tty_struct {
	unsigned int	magic;
	struct termios	termios;
	struct tty_ldisc  *ldisc;
	struct tty_driver *driver;
	const struct tty_operations *ops;
	void *disc_data;
	void *driver_data;
};

/* System present tty_structs */
/* tty_table[0] is registed as console tty */
extern struct tty_struct tty_table[2];

/* TTY Driver Types */
#define TTY_DRIVER_TYPE_SYSTEM		0x0001
#define TTY_DRIVER_TYPE_CONSOLE		0x0002
#define TTY_DRIVER_TYPE_SERIAL		0x0003
#define TTY_DRIVER_TYPE_PTY		0x0004
#define TTY_DRIVER_TYPE_DUMMY		0x0005

void tty_set_operations(struct tty_driver *driver, const struct tty_operations *ops);
int tty_unregister_driver(struct tty_driver *driver);
int tty_register_driver(struct tty_driver *driver);
struct tty_struct *alloc_tty_struct(struct tty_driver *driver, int idx);

void __init tty_init(void);

#endif /* _SANDIX_TTY_H_ */
