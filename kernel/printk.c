/*
 *	Copyright (C) 2015-2016 Yizhou Shan <shan13@purdue.edu>
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

#include <sandix/tty.h>
#include <sandix/kernel.h>
#include <sandix/linkage.h>
#include <sandix/kern_levels.h>

static unsigned char KMBUF[1024];

asmlinkage __printf(1, 2)
int printk(const char *fmt, ...)
{
	va_list args;
	int len;
	
	va_start(args, fmt);
	len = vsnprintf(KMBUF, 1024, fmt, args);
	va_end(args);
	
	/* FIXME
	 * Use line discipline
	 *
	 *	tty_write(NULL, KMBUF, len, NULL);
	 *
	 */
	//tty_table[0].ops->write(&tty_table[0], KMBUF, len);

	tty_write(NULL, KMBUF, len, NULL);

	return len;
}
