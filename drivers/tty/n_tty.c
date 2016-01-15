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

/*
 * This file describes N_TTY line discipline
 */

#include <sandix/compiler.h>
#include <sandix/types.h>
#include <sandix/tty.h>

struct n_tty_data {

};

static ssize_t n_tty_read(struct tty_struct *tty, char __user *buf,
			  size_t count)
{

}

static ssize_t n_tty_write(struct tty_struct *tty, struct file *file
			   const unsigned char __user *buf, size_t count)
{
	
}

static void n_tty_set_termios(struct tty_struct *tty, struct termios *old)
{

}

static void n_tty_close(struct tty_struct *tty)
{

}

static int n_tty_open(struct tty_struct *tty)
{

}

struct tty_ldisc_ops tty_ldisc_N_TTY = {
	.name		= "n_tty",
	.open		= n_tty_open,
	.close		= n_tty_close,
	.read		= n_tty_read,
	.write		= n_tty_write,
	.set_termios	= n_tty_set_termios
};
EXPORT_SYMBOL(tty_ldisc_N_TTY);
