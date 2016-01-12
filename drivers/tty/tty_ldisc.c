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
 * This file describes the line discipline layer
 */

#include <sandix/errno.h>
#include <sandix/kernel.h>
#include <sandix/list.h>
#include <sandix/tty.h>
#include <sandix/mutex.h>
#include <sandix/spinlock.h>

static DEFINE_RAW_SPINLOCK(tty_ldiscs_lock);
static struct tty_ldisc_ops *tty_ldiscs[NR_LDISCS];

int tty_register_ldisc(int disc, struct tty_ldisc_ops *new_ldisc)
{
	unsigned long flags;

	if (disc < N_TTY || disc >= NR_LDISCS)
		return -EINVAL;
	
	raw_spin_lock_irqsave(&tty_ldiscs_lock, flags);
	tty_ldiscs[disc] = new_ldisc;
	new_ldisc->num = disc;
	new_ldisc->refcount = 0;
	raw_spin_unlock_irqrestore(&tty_ldiscs_lock, flags);

	return 0;
}
EXPORT_SYMBOL(tty_register_ldisc);

int tty_unregister_ldisc(int disc)
{
	unsigned long flags;

	if (disc < N_TTY || disc >= NR_LDISCS)
		return -EINVAL;

	raw_spin_lock_irqsave(&tty_ldiscs_lock, flags);
	if (tty_ldisc[disc]->refcount)
		rertun -EBUSY;
	else
		tty_ldisc[disc] = NULL;
	raw_spin_unlock_irqrestore(&tty_ldisc_lock, flags);

	return 0;
}
EXPORT_SYMBOL(tty_unregister_ldisc);

struct tty_ldisc *tty_ldisc_ref_wait(struct tty_struct *tty)
{
	return tty->ldisc;
}
EXPORT_SYMBOL(tty_ldisc_ref_wait);

void tty_ldisc_deref(struct tty_ldisc *ld)
{

}
EXPORT_SYMBOL(tty_ldisc_deref);
