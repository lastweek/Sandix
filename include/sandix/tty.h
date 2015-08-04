/*
 *	include/sandix/tty.h - TTY Layer Architecture
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
#ifndef _SANDIX_TTY_H_
#define _SANDIX_TTY_H_

#include <sandix/console.h>
#include <sandix/types.h>

struct tty_struct;

struct tty_operations {
	ssize_t		(*tty_write)(struct tty_struct *tty);
	ssize_t		(*tty_read)(struct tty_struct *tty);
};

struct tty_struct {
	struct vc_struct	*console;
	struct tty_operations	*ops;
};


void tty_init(void);
#endif /* _SANDIX_TTY_H_ */
