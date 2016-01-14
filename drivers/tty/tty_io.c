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

/*
 * For the benefit of tty drivers
 */
struct termios tty_std_termios = {
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

/*
 * List of all registed tty drivers in system
 */
LIST_HEAD(tty_drivers);
EXPORT_SYMBOL(tty_drivers);

DEFINE_MUTEX(tty_mutex);
EXPORT_SYMBOL(tty_mutex);

/* XXX
 * tty_table[0] used for console_driver
 * tty_table[1] used for dummy_tty
 */
struct tty_struct tty_table[2];

void tty_set_operations(struct tty_driver *driver,
			const struct tty_operations *ops)
{
	driver->ops = ops;
}
EXPORT_SYMBOL(tty_set_operations);

/*
 * Called by a tty driver to unregister itself
 */
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

/*
 * Called by a tty driver to register itself
 */
int tty_register_driver(struct tty_driver *driver)
{
	if (!driver)
		return -EINVAL;

	mutex_lock(&tty_mutex);
	list_add(&driver->tty_drivers, &tty_drivers);
	mutex_unlock(&tty_mutex);

	driver->flags |= TTY_DRIVER_INSTALLED;

	return 0;
}
EXPORT_SYMBOL(tty_register_driver);

void tty_print_drivers(void)
{
	struct tty_driver *driver;

	printk("Registed tty drivers:\n\r");

	list_for_each_entry(driver, &tty_drivers, tty_drivers) {
		printk("\tDriver name: %s\n\r", driver->name);
		printk("\tDriver type: ");
		switch (driver->type) {
		case TTY_DRIVER_TYPE_CONSOLE:
			printk("CONSOLE\n\t"); break;
		case TTY_DRIVER_TYPE_SERIAL:
			printk("SERIAL\n\t"); break;
		case TTY_DRIVER_TYPE_PTY:
			printk("PTY\n\t"); break;
		case TTY_DRIVER_TYPE_DUMMY:
			printk("DUMMY\n\t"); break;
		default:
			printk("UNKNOWN\n\t");
		}
	}
}
EXPORT_SYMBOL(tty_print_drivers);

/**
 * alloc_tty_struct
 * @driver:	driver to hook
 * @idx:	idx of the tty_struct table
 * @Return:	!%NULL on success
 *
 * This function allocates and initializes a tty struct.
 */
struct tty_struct *alloc_tty_struct(struct tty_driver *driver, int idx)
{
	struct tty_struct *tty;

	/* XXX
	 *
	 *	tty = kzalloc(sizeof(*tty), GFP_KERNEL);
	 *	if (!tty)
	 *		return NULL;
	 */
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

/*
 * Write one byte to the tty using the provided put_char method if present.
 * Return number of characters successfully output
 */
int tty_put_char(struct tty_struct *tty, unsigned char ch)
{
	if (tty->ops->put_char)
		return tty->ops->put_char(tty, ch);
	return tty->ops->write(tty, &ch, 1);
}
EXPORT_SYMBOL(tty_put_char);

/*
 * Split writes up in sane blocksizes to avoid Denial-Of-Sevice type attacks
 */
static ssize_t do_tty_write(
	ssize_t (*write)(struct tty_struct *, struct file *, const unsigned char *, size_t),
	struct tty_struct *tty,
	struct file *file,
	const char __user *buf,
	size_t count)
{
	ssize_t ret;
}

/**
 * tty_write		-	Write method for tty device file
 * @file:	tty file pointer
 * @buf:	user data to write
 * @count:	bytes to write
 * @ppos:	unused
 *
 * Write data to a tty device through line discipline
 */
ssize_t tty_write(struct file *file, const char __user *buf,
		size_t count, loff_t **ppos)
{
	struct tty_struct *tty;
	struct tty_ldisc *ld;
	ssize_t ret;

#if 0
	tty = file_tty(file);
#endif

	tty = &tty_table[0];
	if (!tty || !tty->ops->write)
		return -EIO;

	ld = tty_ldisc_ref_wait(tty);
	if (!ld->ops->write)
		return -EIO;
	else
		ret = do_tty_write(ld->ops->write, tty, file, buf, count);
	tty_ldisc_deref(ld);

	return ret;
}

void __init tty_init(void)
{
	tty_ldisc_begin();

	/*
	 * This function will register tty_driver of console
	 * We are safe to output to screen after this call.
	 */
	console_init();
}
