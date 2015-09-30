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
 * This file describes the simple TTY layer in Sandix.
 */

#include <sandix/errno.h>
#include <sandix/compiler.h>
#include <sandix/console.h>
#include <sandix/export.h>
#include <sandix/tty.h>
#include <sandix/types.h>
#include <sandix/major.h>
#include <sandix/list.h>
#include <sandix/mutex.h>
#include <sandix/magic.h>
#include <sandix/kref.h>
#include <sandix/fs.h>

struct termios tty_std_termios = {	/* for the benefit of tty drivers  */
	.c_iflag = ICRNL | IXON,
	.c_oflag = OPOST | ONLCR,
	.c_cflag = B38400 | CS8 | CREAD | HUPCL,
	.c_lflag = ISIG | ICANON | ECHO | ECHOE | ECHOK |
		   ECHOCTL | ECHOKE | IEXTEN,
	.c_cc = INIT_C_CC,
	.c_ispeed = 38400,
	.c_ospeed = 38400
};
EXPORT_SYMBOL(tty_std_termios);

LIST_HEAD(tty_drivers);
EXPORT_SYMBOL(tty_drivers);

DEFINE_MUTEX(tty_mutex);
EXPORT_SYMBOL(tty_mutex);

/* FIXME
 * tty_table[0] used for console_driver
 * tty_table[1] used for dummy_tty
 */
struct tty_struct tty_table[2];

/**
 * tty_write - Write method for tty device
 * @file:	tty file pointer
 * @buf:	user data to write
 * @count:	bytes to write
 * Return:	negative on error
 *
 * Write data from user space to tty device. Line discipline will be
 * invoked first before the real tty driver write method.
 */
ssize_t tty_write(struct file *file, const char __user *buf, size_t count)
{
	struct tty_struct *tty;
	ssize_t ret;

	/*FIXME*/
	tty = &tty_table[0];

	if (!tty || !tty->ops->write
		 || !tty->ldisc->ops->write)
		return -EIO;
	
}

void tty_set_operations(struct tty_driver *driver,
			const struct tty_operations *ops)
{
	driver->ops = ops;
}
EXPORT_SYMBOL(tty_set_operations);

int tty_unregister_driver(struct tty_driver *driver)
{
	if (!driver)
		return -EINVAL;

	mutex_lock(&tty_mutex);
	list_del(&driver->tty_drivers);
	mutex_unlock(&tty_mutex);

	return 0;
}
EXPORT_SYMBOL(tty_unregister_driver);

int tty_register_driver(struct tty_driver *driver)
{
	if (!driver)
		return -EINVAL;

	mutex_lock(&tty_mutex);
	list_add(&driver->tty_drivers, &tty_drivers);
	mutex_unlock(&tty_mutex);

	return 0;
}
EXPORT_SYMBOL(tty_register_driver);

struct tty_struct *alloc_tty_struct(struct tty_driver *driver, int idx)
{
	struct tty_struct *tty;

	/* Should use kmalloc() to allocate tty_struct
	   dynamically rather than hardcoded table. */
	tty = &tty_table[idx];

	/* Initialize tty_struct */
	tty->magic = TTY_STRUCT_MAGIC;
	tty->termios = driver->init_termios;
	tty->driver = driver;
	tty->ops = driver->ops;
	kref_init(&tty->kref);

	return tty;
}
EXPORT_SYMBOL(alloc_tty_struct);

void __init tty_init(void)
{
	/* Register tty driver for console */
	console_init();
}
