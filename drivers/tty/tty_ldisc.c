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
#include <sandix/rwsem.h>
#include <sandix/spinlock.h>
#include <sandix/slab.h>

/*
 * Using a static array to store registed discipline, a simple spinlock is used
 * to protect this array. All manipulations of this array *must* be carried
 * out with this spinlock held.
 */
static DEFINE_SPINLOCK(tty_ldiscs_lock);
static struct tty_ldisc_ops *tty_ldiscs[NR_LDISCS];

/**
 *	tty_register_ldisc	-	install a line discipline
 *	@disc: ldisc number
 *	@new_ldisc: pointer to the ldisc object
 *
 *	Installs a new line discipline into the kernel. The discipline
 *	is set up as unreferenced and then made available to the kernel
 *	from this point onwards.
 */
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

/**
 *	tty_unregister_ldisc	-	unload a line discipline
 *	@disc: ldisc number
 *	@new_ldisc: pointer to the ldisc object
 *
 *	Remove a line discipline from the kernel providing it is not
 *	currently in use.
 */
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

/**
 *	get_ldops	-	Take a reference of ldisc ops
 *	@disc: ldisc number
 *
 *	Return the pointer to the line discipline operation structure,
 *	increase refcount of it.
 */
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

/**
 *	tty_ldisc_get	-	Take a reference of a line discipine
 *	@tty: the tty in move
 *	@disc: disc number
 *
 *	Takes a reference of a line discipline op, deals with refcount.
 *	Returns %NULL if line discipline op is not available. Return the
 *	pointer to the tty_ldisc if succeed, and increases the refcount
 *	of the line discipline op.
 */
static struct tty_ldisc *tty_ldisc_get(struct tty_struct *tty, int disc)
{
	struct tty_ldisc *ld;
	struct tty_ldisc_ops *ldops;

	if (disc < N_TTY || disc >= NR_LDISCS)
		return ERR_PTR(-EINVAL);

	ldops = get_ldops(disc);
	if (IS_ERR(ldops))
		return ERR_CAST(ldops);

	ld = kmalloc(sizeof(struct tty_ldisc), 1);
	if (!ld) {
		put_ldops(ldops);
		return ERR_PTR(-ENOMEM);
	}

	ld->ops = ldops;
	ld->tty = tty;

	return ld;
}

static void tty_ldisc_put(struct tty_ldisc *ld)
{
	if (WARN_ON(!ld))
		return;

	put_ldops(ld->ops);
	kfree(ld);
}

static void tty_set_termios_ldisc(struct tty_struct *tty, int disc)
{
	down_write(&tty->termios_rwsem);
	tty->termios.c_line = disc;
	up_write(&tty->termios_rwsem);
}

/**
 *	tty_ldisc_open	-	Open tty's line discipline
 *	@tty: tty being open
 *	@ld: ldisc being open
 *
 *	Calls open() function of line discipline operations. The low
 *	level open() will allocate memory, and tty is safe to use after
 *	this call.
 */
int tty_ldisc_open(struct tty_struct *tty, struct tty_ldisc *ld)
{
	int ret = 0;

	/*XXX tty->flags */
	if (ld->ops->open) {
		ret = ld->ops->open(tty);
		if (ret) ;
	}
	return ret;
}
EXPORT_SYMBOL(tty_ldisc_open);

/**
 *	tty_ldisc_close	-	Close tty's line discipline
 *	@tty: tty being closed
 *	@ld: ldisc being closed
 *
 *	Calls close() function of line discipline operations. The low
 *	level close() will cleanup allocated memory, but the tty and
 *	line discipline operations are still bonded.
 */
void tty_ldisc_close(struct tty_struct *tty, struct tty_ldisc *ld)
{
	/*XXX tty->flags */
	if (ld->ops->close)
		ld->ops->close(tty);
}
EXPORT_SYMBOL(tty_ldisc_close);

/**
 *	tty_ldisc_restore	-	helper for tty ldisc change
 *	@tty: tty to recover
 *	@old: previous ldisc
 *
 *	Restore the previous line discipline or N_TTY when a line discipline
 *	change fails due to an open error
 */
static void tty_ldisc_restore(struct tty_struct *tty, struct tty_ldisc *old)
{
	int ret;
	struct tty_ldisc *new_ldisc;

	/* There is an outstanding reference here so this is safe */
	old = tty_ldisc_get(tty, old->ops->num);
	WARN_ON(IS_ERR(old));
	tty->ldisc = old;
	tty_set_termios_ldisc(tty, old->ops->num);
	if (tty_ldisc_open(tty, old) < 0) {
		tty_ldisc_put(old);

		/* This driver is always present */
		new_ldisc = tty_ldisc_get(tty, N_TTY);
		if (IS_ERR(new_ldisc))
			panic("n_tty: get");

		tty->ldisc = new_ldisc;
		tty_set_termios_ldisc(tty, N_TTY);
		
		ret = tty_ldisc_open(tty, new_ldisc);
		if (ret < 0)
			panic("n_tty: open");
	}
}

/**
 *	tty_ldisc_change	-	change line discipline
 *	@tty: tty to change
 *	@disc: new line discipine number
 *
 *	Change the line discipline of the tty. The tty must have an old
 *	ldisc attached to, we must handle it properly. Sandix implement
 *	it quite simple, but please look at linux, see how complex it is!
 *	XXX tty shoud be locked
 */
int tty_ldisc_change(struct tty_struct *tty, int disc)
{
	int ret;
	struct tty_ldisc *old_ldisc, *new_ldisc;

	BUG_ON(!tty->ldisc);
	if (tty->ldisc->ops->num == disc)
		return 0;

	tty_lock(tty);
	new_ldisc = tty_ldisc_get(tty, disc);
	if (IS_ERR(new_ldisc)) {
		tty_unlock(tty);
		return PTR_ERR(new_ldisc);
	}

	old_ldisc = tty->ldisc;

	/* Close the old one */
	tty_ldisc_close(tty, old_ldisc);

	/* Set up the new one */
	tty->ldisc = new_ldisc;
	tty_set_termios_ldisc(tty, disc);

	ret = tty_ldisc_open(tty, new_ldisc);
	if (ret) {
		/* Back to the old one or N_TTY */
		tty_ldisc_put(new_ldisc);
		tty_ldisc_restore(tty, old_ldisc);
	}

	/*
	 * At this point we hold a reference to the new ldisc and a
	 * reference to the old ldisc, or we hold two references to
	 * the old ldisc (if it was restored as part of error cleanup
	 * above). In either case, releasing a single reference from
	 * the old ldisc is correct.
	 */
	tty_ldisc_put(old_ldisc);

	tty_unlock(tty);
	return ret;
}
EXPORT_SYMBOL(tty_ldisc_change);

/*TODO*/
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

/**
 *	tty_ldisc_init	-	set n_tty as new tty's ldisc
 *	@tty: the new tty
 *
 *	Set up the line discipline object for a newly allocated tty.
 *	The default line discipline is N_TTY, we depend on it!
 */
void tty_ldisc_init(struct tty_struct *tty)
{
	struct tty_ldisc *ld;

	ld = tty_ldisc_get(tty, N_TTY);
	if (IS_ERR(ld))
		panic("n_tty: missing");
	tty->ldisc = ld;
}
EXPORT_SYMBOL(tty_ldisc_init);

void tty_ldisc_deinit(struct tty_struct *tty)
{
	tty_ldisc_put(tty->ldisc);
	tty->ldisc = NULL;
}
EXPORT_SYMBOL(tty_ldisc_deinit);

void tty_ldisc_begin(void)
{
	/* N_TTY is the default line discipline, */
	tty_register_ldisc(N_TTY, &tty_ldisc_N_TTY);
}
