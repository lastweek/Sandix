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

#include <sandix/kernel.h>
#include <sandix/list.h>
#include <sandix/tty.h>
#include <sandix/mutex.h>

int tty_register_ldisc()

int tty_unregister_ldisc()

int tty_set_ldisc()

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
