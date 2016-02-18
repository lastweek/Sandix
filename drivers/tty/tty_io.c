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
 * This file describes the tty layer
 */

#include <sandix/fs.h>
#include <sandix/bug.h>
#include <sandix/tty.h>
#include <sandix/list.h>
#include <sandix/kref.h>
#include <sandix/errno.h>
#include <sandix/major.h>
#include <sandix/mutex.h>
#include <sandix/magic.h>
#include <sandix/types.h>
#include <sandix/export.h>
#include <sandix/console.h>
#include <sandix/uaccess.h>
#include <sandix/compiler.h>

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

/* list of all registed drivers */
LIST_HEAD(tty_drivers);
DEFINE_MUTEX(tty_mutex);

EXPORT_SYMBOL(tty_drivers);
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

/**
 *	tty_register_driver	-	install a tty driver
 *	@driver: who wants to install
 *
 *	Called by a tty driver to register itself, it would be
 *	inserted into registed tty driver list
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

/**
 *	tty_unregister_driver	-	unload a tty driver
 *	@driver: who wants to unload
 *
 *	Called by a tty driver to unregister itself, it would be
 *	removed from registed tty driver list
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

void tty_print_drivers(void)
{
	struct tty_driver *driver;
	const unsigned char *type_name[] = {
		"UNKNOWN", "SYSREM", "CONSOLE", "SERIAL", "PTY", "DUMMY"
	};

	printk("Registed tty drivers:\n");

	list_for_each_entry(driver, &tty_drivers, tty_drivers) {
		printk("\tDriver name: %s\n", driver->name);
		printk("\tDriver type: ");
		switch (driver->type) {
		case TTY_DRIVER_TYPE_SYSTEM:
		case TTY_DRIVER_TYPE_CONSOLE:
		case TTY_DRIVER_TYPE_SERIAL:
		case TTY_DRIVER_TYPE_PTY:
		case TTY_DRIVER_TYPE_DUMMY:
			printk("%s\n", type_name[driver->type]);
			break;
		default:
			printk("UNKNOWN\n");
		}
	}
}

/**
 *	tty_lock	-	lock whole tty struct
 *	@tty: to lock
 *
 *	This function locks the whole tty struct, and gets the kref.
 *	It is big mutex lock.
 */
void tty_lock(struct tty_struct *tty)
{
	if (tty->magic != TTY_STRUCT_MAGIC) {
		pr_err("L Bad Magic");
		WARN_ON(1);
		return;
	}
	kref_get(&tty->kref);
	mutex_lock(&tty->tty_lock);
}
EXPORT_SYMBOL(tty_lock);

void tty_unlock(struct tty_struct *tty)
{
	if (tty->magic != TTY_STRUCT_MAGIC) {
		pr_err("U Bad Magic");
		WARN_ON(1);
		return;
	}
	mutex_unlock(&tty->tty_lock);
	kref_put(&tty->kref);
}
EXPORT_SYMBOL(tty_unlock);

/*XXX delete this after kmalloc done */
static struct tty_ldisc tmp_ldisc;

/**
 *	alloc_tty_struct	-	allocate a new tty struct
 *	@driver: driver to hook
 *	@idx: idx of the tty_struct table
 *
 *	This function allocates and initializes a tty struct. All fields
 *	inside tty_struct are initialized properly. Also note that the line
 *	discipline is set to N_TTY, which is the default terminal ldisc.
 */
struct tty_struct *alloc_tty_struct(struct tty_driver *driver, int idx)
{
	struct tty_struct *tty;

#if 0
	tty = kzalloc(sizeof(struct tty_struct), GFP_KERNEL);
	if (!tty)
		return ERR_PTR(-ENOMEM);

	tty_ldisc_init(tty);
#endif

	tty = &tty_table[idx];
	tmp_ldisc.ops = &tty_ldisc_N_TTY;
	tmp_ldisc.tty = tty;
	tty->ldisc = &tmp_ldisc;

	/* XXX Should be removed */
	tty_ldisc_open(tty, tty->ldisc);

	tty->magic = TTY_STRUCT_MAGIC;
	tty->index = 0;
	tty->flags = 0;

	spin_lock_init(&tty->flow_lock);
	tty->stopped = 0;

	mutex_init(&tty->tty_lock);
	kref_init(&tty->kref);

	rwsem_init(&tty->termios_rwsem);
	tty->termios = driver->init_termios;

	tty->driver = driver;
	tty->ops = driver->ops;

	rwsem_init(&tty->ldisc_rwsem);

	tty->write_cnt = TTY_WRITE_BUF_SIZE;

	return tty;
}
EXPORT_SYMBOL(alloc_tty_struct);

/**
 *	tty_chars_in_buffer	-	characters pending
 *	@tty: terminal
 *
 *	Return the number of bytes of data in the device private
 *	output queue. If no private method is supplied there is assumed
 *	to be no queue on the device.
 */
int tty_chars_in_buffer(struct tty_struct *tty)
{
	if (tty->ops->chars_in_buffer)
		return tty->ops->chars_in_buffer(tty);
	else
		return 0;
}
EXPORT_SYMBOL(tty_chars_in_buffer);

/**
 *	tty_write_room		-	write queue space
 *	@tty: terminal
 *
 *	Return the number of bytes that can be queued to this device
 *	at the present time. The result should be treated as a guarantee
 *	and the driver cannot offer a value it later shrinks by more than
 *	the number of bytes written. If no method is provided 2K is always
 *	returned and data may be lost as there will be no flow control.
 */
int tty_write_room(struct tty_struct *tty)
{
	if (tty->ops->write_room)
		return tty->ops->write_room(tty);
	return 2048;
}
EXPORT_SYMBOL(tty_write_room);

/**
 *	tty_driver_flush_buffer	-	discard internal buffer
 *	@tty: terminal
 *
 *	Discard the internal output buffer for this device. If no method
 *	is provided then either the buffer cannot be hardware flushed or
 *	there is no buffer driver side.
 */
void tty_driver_flush_buffer(struct tty_struct *tty)
{
	if (tty->ops->flush_buffer)
		tty->ops->flush_buffer(tty);
}
EXPORT_SYMBOL(tty_driver_flush_buffer);

/**
 *	tty_put_char	-	write one char to tty device
 *	@tty: tty to write
 *	@ch: character to output
 *
 *	Write one byte to the tty using the provided put_char method
 *	if present. Return number of characters successfully output.
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

	/*
	 * We chunk up writes into a temporary buffer. This
	 * simplifies low-level drivers immensely, since they
	 * don't have locking issues and user mode accesses.
	 *
	 * The default buffer-size is TTY_WRITE_BUF_SIZE = 2KB
	 */
	if (count > tty->write_cnt) {
		WARN(1, "tty_write: bytes pruned");
		count = tty->write_cnt;
	}

	if (copy_from_user(tty->write_buf, buf, count))
		return -EFAULT;
	
	ret = write(tty, file, tty->write_buf, count);
	return ret;
}

/**
 *	tty_write	-	write method for tty device file
 *	@file: tty file pointer
 *	@buf: user data to write
 *	@count: bytes to write
 *	@ppos: unused
 *
 *	Write data to a tty device through line discipline
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
	/*
	 * This function calls line discipline layer to register N_TTY
	 * ops. N_TTY is the default ldisc, which *must* be available.
	 */
	tty_ldisc_begin();

	/*
	 * This function will register tty_driver of console
	 * We are safe to output to screen after this call.
	 */
	console_init();
}
