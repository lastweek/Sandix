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
 * This file describes simple TTY layer in Sandix.
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

LIST_HEAD(tty_drivers);

DEFINE_MUTEX(tty_mutex);

/* FIXME No Table! Allocate dynamically */
/* tty_table[0] used for console_driver */
/* tty_table[1] used for dummy_tty */
struct tty_struct tty_table[2];

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
	tty->driver = driver;
	tty->ops = driver->ops;

	return tty;
}
EXPORT_SYMBOL(alloc_tty_struct);

void __init tty_init(void)
{
	console_init();
}
