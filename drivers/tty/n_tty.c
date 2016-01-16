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
 *	This file describes N_TTY line discipline. All functions inside this
 *	file must not make any mistaks, since this is the default line
 *	discipline in the system. System would move back to N_TTY if other line
 *	disciplines make any mistakes..
 */

#include <sandix/bug.h>
#include <sandix/tty.h>
#include <sandix/errno.h>
#include <sandix/types.h>
#include <sandix/rwsem.h>
#include <sandix/compiler.h>

struct n_tty_data {
	struct mutex	output_mutex;
};

static ssize_t n_tty_read(struct tty_struct *tty, char __user *buf,
			  size_t count)
{

}

 
/**
 *	process_output_block	-	block post processor
 *	@tty: terminal device
 *	@buf: character buffer
 *	@count: number of bytes to output
 *
 *	Output a block of characters with OPOST processing.
 *	Returns the number of characters output.
 *
 *	This path is used to speed up block console writes, among other
 *	things when processing blocks of output data. It handles only
 *	the simple cases normally found and helps to generate blocks of
 *	symbols for the console driver and thus improve performance.
 *
 *	Locking: output_mutex to protect column state and space left
 *		 (also, this is called from n_tty_write under the
 *		  tty layer write lock)
 */
static ssize_t process_output_block(struct tty_struct *tty,
				    const unsigned char *b, size_t count)
{

}

/**
 *	process_output		-	output post processor
 *	@c: character
 *	@tty: terminal device
 *
 *	Output one character with OPOST processing. Returns -1 when the
 *	output device is full and the character must be retried.
 *
 *	Locking: output_mutex to protect column state and space left
 *		 (also, this is called from n_tty_write under the
 *		  tty layer write lock)
 */
static int process_output(unsigned char c, struct tty_struct *tty)
{
	int space, retval;
	struct n_tty_data *ldata = tty->ldisc_data;

	mutex_lock(&ldata->output_mutex);
	space = tty_write_room(tty);
	retval = do_output_char(c, tty, space);
	mutex_unlock(&ldata->output_mutex);

	if (retval < 0)
		return -1;
	else
		return 0;
}


static ssize_t n_tty_write(struct tty_struct *tty, struct file *file,
			   const unsigned char *buf, size_t count)
{
	ssize_t retval;
	const unsigned char *b = buf;

	down_read(&tty->termios_rwsem);

	if (O_OPOST(tty)) {
		while (count > 0) {
			ssize_t num;
			unsigned char ch;

			num = process_output_block(tty, b, count);
			if (num < 0) {
				if (num == -EAGAIN)
					continue;
				retval = num;
				goto break_out;
			}

			b += num;
			count -= num;

			if (count == 0)
				break;

			ch = *b;
			if (process_output(ch, tty) < 0)
				continue;

			b++;
			count--;
		}
		if (tty->ops->flush_chars)
			tty->ops->flush_chars(tty);
	} else {
		struct n_tty_data *ldata = tty->ldisc_data;
		int c;

		while (count > 0) {
			mutex_lock(&ldata->output_mutex);
			c = tty->ops->write(tty, buf, count);
			mutex_unlock(&ldata->output_mutex);

			if (c < 0) {
				retval = c;
				goto break_out;
			}

			if (!c)
				break;

			buf += c;
			count -= c;
		}
	}

break_out:

	up_read(&tty->termios_rwsem);
	return retval;
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
