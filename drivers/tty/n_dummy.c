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
 * This file describes a dummy line discipline.
 */

#include <sandix/kernel.h>
#include <sandix/types.h>
#include <sandix/tty.h>

static int n_dummy_read(struct tty_struct *tty, char __user *buf, size_t count)
{

}

static ssize_t n_dummy_write(struct tty_struct *tty, const char __user *buf,
			   size_t count)
{

}

static void n_dummy_set_termios(struct tty_struct *tty, struct termios *old)
{

}

const struct tty_ldisc_ops tty_ldisc_N_DUMMY = {
	.name		= "n_dummy",
	.read		= n_dummy_read,
	.write		= n_dummy_write,
	.set_termios	= n_dummy_set_termios
};
EXPORT_SYMBOL(tty_ldisc_N_DUMMY);
