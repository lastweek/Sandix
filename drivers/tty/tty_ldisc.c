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

#include <sandix/err.h>
#include <sandix/errno.h>
#include <sandix/kernel.h>
#include <sandix/list.h>
#include <sandix/tty.h>
#include <sandix/mutex.h>
#include <sandix/spinlock.h>

/*
 * Using a static array to store registed discipline, a simple spinlock is used
 * to protect this array. All manipulations of this array *must* be carried
 * out with this spinlock held.
 */
static DEFINE_SPINLOCK(tty_ldiscs_lock);
static struct tty_ldisc_ops *tty_ldiscs[NR_LDISCS];

int tty_register_ldisc(int disc, struct tty_ldisc_ops *new_ldisc)
{
	unsigned long flags;

	if (disc < N_TTY || disc >= NR_LDISCS)
		return -EINVAL;
	
	spin_lock_irqsave(&tty_ldiscs_lock, flags);
	tty_ldiscs[disc] = new_ldisc;
	new_ldisc->num = disc;
	new_ldisc->refcount = 0;
	spin_unlock_irqrestore(&tty_ldiscs_lock, flags);

	return 0;
}
EXPORT_SYMBOL(tty_register_ldisc);

int tty_unregister_ldisc(int disc)
{
	unsigned long flags;

	if (disc < N_TTY || disc >= NR_LDISCS)
		return -EINVAL;

	spin_lock_irqsave(&tty_ldiscs_lock, flags);
	if (tty_ldiscs[disc]->refcount)
		return -EBUSY;
	else
		tty_ldiscs[disc] = NULL;
	spin_unlock_irqrestore(&tty_ldiscs_lock, flags);

	return 0;
}
EXPORT_SYMBOL(tty_unregister_ldisc);

static struct tty_ldisc_ops *get_ldops(int disc)
{
	unsigned long flags;
	struct tty_ldisc_ops *ldops, *ret;

	spin_lock_irqsave(&tty_ldiscs_lock, flags);
	ret = ERR_PTR(-EINVAL);
	ldops = tty_ldiscs[disc];
	if (ldops) {
		ldops->refcount++;
		ret = ldops;
	}
	spin_unlock_irqrestore(&tty_ldiscs_lock, flags);

	return ret;
}

static void put_ldops(struct tty_ldisc_ops *ldops)
{
	unsigned long flags;

	spin_lock_irqsave(&tty_ldiscs_lock, flags);
	ldops->refcount--;
	spin_unlock_irqrestore(&tty_ldiscs_lock, flags);
}

struct tty_ldisc *tty_ldisc_ref_wait(struct tty_struct *tty)
{
	return tty->ldisc;
}
EXPORT_SYMBOL(tty_ldisc_ref_wait);

/*TODO*/
void tty_ldisc_deref(struct tty_ldisc *ld)
{

}
EXPORT_SYMBOL(tty_ldisc_deref);


void tty_ldisc_begin(void)
{
	/* N_TTY is the default line discipline, */
	tty_register_ldisc(N_TTY, &tty_ldisc_N_TTY);
}
