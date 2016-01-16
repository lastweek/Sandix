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
#include <sandix/slab.h>
#include <sandix/ctype.h>
#include <sandix/errno.h>
#include <sandix/types.h>
#include <sandix/mutex.h>
#include <sandix/compiler.h>

struct n_tty_data {

	/* protected by output_lock */
	unsigned int		column;
	unsigned int		canon_column;

	struct mutex		output_lock;
};

static ssize_t n_tty_read(struct tty_struct *tty, char __user *buf,
			  size_t count)
{
	return 0;
}

/**
 *	do_output_char			-	output one character
 *	@c: character
 *	@tty: terminal device
 *	@space: space available in tty driver write buffer
 *
 *	This is a helper function that handles one output character
 *	(including special characters like TAB, CR, LF, etc.),
 *	doing OPOST processing and putting the results in the
 *	tty driver's write buffer.
 *
 *	Note that Sandix currently ignores TABDLY, CRDLY, VTDLY, FFDLY
 *	and NLDLY.  They simply aren't relevant in the world today.
 *	If you ever need them, add them here.
 *
 *	Returns the number of bytes of buffer space used or -1 if
 *	no space left.
 *
 *	Locking: should be called under the output_lock to protect
 *		 the column state and space left in the buffer
 */
static int do_output_char(unsigned char c, struct tty_struct *tty, int space)
{
	struct n_tty_data *ldata = tty->ldisc_data;
	int spaces;

	if (!space)
		return -1;

	switch (c) {
	case '\n':
		if (O_ONLRET(tty)) {
			/* c = '\r'; */
			ldata->column = 0;
		}
		if (O_ONLCR(tty)) {
			if (space < 2)
				return -1;
			ldata->canon_column = ldata->column = 0;
			tty->ops->write(tty, "\r\n", 2);
			return 2;
		}
		ldata->canon_column = ldata->column;
		break;
	case '\r':
		if (O_ONOCR(tty) && ldata->column == 0)
			return 0;
		if (O_OCRNL(tty)) {
			c = '\n';
			if (O_ONLRET(tty))
				ldata->canon_column = ldata->column = 0;
			break;
		}
		ldata->canon_column = ldata->column = 0;
		break;
	case '\t':
		spaces = 8 - (ldata->column & 7);
		if (O_TABDLY(tty) == XTABS) {
			if (space < spaces)
				return -1;
			ldata->column += spaces;
			tty->ops->write(tty, "        ", spaces);
			return spaces;
		}
		ldata->column += spaces;
		break;
	case '\b':
		if (ldata->column > 0)
			ldata->column--;
		break;
	default:
		if (!iscntrl(c)) {
			if (O_OLCUC(tty))
				c = toupper(c);
		}
		break;
	}

	tty_put_char(tty, c);
	return 1;
}

/**
 *	process_output		-	output post processor
 *	@c: character
 *	@tty: terminal device
 *
 *	Output one character with OPOST processing. Returns -1 when the
 *	output device is full and the character must be retried.
 *
 *	Locking: output_lock to protect column state and space left
 *		 (also, this is called from n_tty_write under the
 *		  tty layer write lock)
 */
static int process_output(unsigned char ch, struct tty_struct *tty)
{
	int space, retval;
	struct n_tty_data *ldata = tty->ldisc_data;

	mutex_lock(&ldata->output_lock);
	space = tty_write_room(tty);
	retval = do_output_char(ch, tty, space);
	mutex_unlock(&ldata->output_lock);

	if (retval < 0)
		return -1;
	else
		return 0;
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
 *	Locking: output_lock to protect column state and space left
 *		 (also, this is called from n_tty_write under the
 *		  tty layer write lock)
 */
static ssize_t process_output_block(struct tty_struct *tty,
				    const unsigned char *buf, size_t count)
{
	struct n_tty_data *ldata = tty->ldisc_data;
	const unsigned char *b;
	size_t i;
	int space;

	mutex_lock(&ldata->output_lock);
	
	space = tty_write_room(tty);
	if (!space) {
		mutex_unlock(&ldata->output_lock);
		return 0;
	}

	if (count > space)
		count = space;

	for (i = 0, b = buf; i < count; i++, b++) {
		unsigned char ch = *b;

		switch (ch) {
		case '\n':
			if (O_ONLRET(tty))
				ldata->column = 0;
			if (O_ONLCR(tty))
				goto break_out;
			ldata->canon_column = ldata->column;
			break;
		case '\r':
			if (O_ONOCR(tty) && ldata->column == 0)
				goto break_out;
			if (O_OCRNL(tty))
				goto break_out;
			ldata->canon_column = ldata->column = 0;
			break;
		case '\t':
			goto break_out;
		case '\b':
			if (ldata->column > 0)
				ldata->column--;
			break;
		default:
			if (!iscntrl(ch) && O_OLCUC(tty))
				goto break_out;
			break;
		}
	}
break_out:
	i = tty->ops->write(tty, buf, i);

	mutex_unlock(&ldata->output_lock);
	return i;
}

static ssize_t n_tty_write(struct tty_struct *tty, struct file *file,
			   const unsigned char *buf, size_t count)
{
	ssize_t retval = 0;
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
			mutex_lock(&ldata->output_lock);
			c = tty->ops->write(tty, b, count);
			mutex_unlock(&ldata->output_lock);

			if (c < 0) {
				retval = c;
				goto break_out;
			}

			if (!c)
				break;

			b += c;
			count -= c;
		}
	}

break_out:
	up_read(&tty->termios_rwsem);
	
	if (b - buf != count)
		/*TODO*/;

	return (b - buf) ? (b - buf) : retval;
}

static void n_tty_set_termios(struct tty_struct *tty, struct termios *old)
{

}

static void n_tty_close(struct tty_struct *tty)
{
	struct n_tty_data *ldata = tty->ldisc_data;
	kfree(ldata);
	tty->ldisc_data = NULL;
}

/* XXX delete this when malloc finished */
static struct n_tty_data tmp_ldata;

static int n_tty_open(struct tty_struct *tty)
{
	struct n_tty_data *ldata;

#if 0
	ldata = kmalloc(sizeof(struct n_tty_data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;
#endif
	ldata = &tmp_ldata;

	mutex_init(&ldata->output_lock);

	tty->ldisc_data = ldata;

	return 0;
}

struct tty_ldisc_ops tty_ldisc_N_TTY = {
	.name		= "n_tty",
	.open		= n_tty_open,
	.close		= n_tty_close,
	.read		= n_tty_read,
	.write		= n_tty_write,
	.set_termios	= n_tty_set_termios
};
