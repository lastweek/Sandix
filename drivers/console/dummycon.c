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

#include <sandix/console.h>

#define DUMMY_COLUMNS	80
#define DUMMY_ROWS	25

static void dummycon_startup(void)
{
	return;
}

static int dummycon_init(struct vc_struct *vc)
{
	vc->vc_cols = DUMMY_COLUMNS;
	vc->vc_rows = DUMMY_ROWS;

	return 0;
}

static int dummycon_dummy(void)
{
	return 0;
}

#define DUMMY	(void *)dummycon_dummy

const struct con_driver dummy_con = {
    .con_startup	=	dummycon_startup,
    .con_init		=	dummycon_init,
    .con_deinit		=	DUMMY,
    .con_clear		=	DUMMY,
    .con_putc		=	DUMMY,
    .con_putcs		=	DUMMY,
    .con_cursor		=	DUMMY,
    .con_scroll		=	DUMMY,
    .con_set_color	=	DUMMY
};
