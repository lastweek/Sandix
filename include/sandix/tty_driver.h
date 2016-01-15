/*
 *	Copyright (C) 2015-2016 Yizhou Shan <shanyizhou@ict.ac.cn>
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

#ifndef _SANDIX_TTY_DRIVE_H_
#define _SANDIX_TTY_DRIVE_H_

#ifndef _SANDIX_TTY_H_
# error "Please do not include this file directly"
#endif

struct tty_struct;

struct tty_operations {
	int	(*write)(struct tty_struct *tty, const unsigned char *buf, int count);
	int	(*put_char)(struct tty_struct *tty, unsigned char ch);
};

/**
 * struct tty_driver
 * @name:		Nick name
 * @flags:		flags of tty driver
 * @type:		Type of this tty driver(See below)
 * @major:		Major number of this tty driver
 * @minor_start:	Starting minor number of this tty driver
 * @num:		Number of devices allocated
 * @init_termios:	Termios of this driver
 * @kref:		Reference count
 * @tty_drivers:	Linked list of all registed tty driver
 * @ops:		Hardware-operations of this tty driver
 *
 * The driver's job is to format data that is sent to it in a manner that the
 * hardware can understand, and receive data from the hardware.
 */
struct tty_driver {
	const char	*name;		
	unsigned int	flags;
	unsigned int	type;		
	unsigned int	major;		
	unsigned int	minor_start;	
	unsigned int	num;		
	struct termios	init_termios;
	struct kref	kref;
	struct list_head tty_drivers;
	const struct tty_operations *ops;
};

/*
 * TTY driver flags
 */
#define TTY_DRIVER_INSTALLED		0x0001
#define TTY_DRIVER_RESET_TERMIOS	0x0002

/*
 * TTY driver types
 */
#define TTY_DRIVER_TYPE_SYSTEM	0x0001
#define TTY_DRIVER_TYPE_CONSOLE	0x0002
#define TTY_DRIVER_TYPE_SERIAL	0x0003
#define TTY_DRIVER_TYPE_PTY	0x0004
#define TTY_DRIVER_TYPE_DUMMY	0x0005

#endif /* _SANDIX_TTY_DRIVE_H_ */
