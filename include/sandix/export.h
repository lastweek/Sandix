/*
 *	include/sandix/export.h - Export Kernel Symbols
 *
 *	Copyright (C) 2015 Yizhou Shan <shanyizhou@ict.ac.cn>
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

#ifndef _SANDIX_EXPORT_H_
#define _SANDIX_EXPORT_H_

/* TODO 
	Add Module Support First.
	Then we can talk about export symbols.
 */

#define __EXPORT_SYMBOL(sym)	extern typeof(sym) sym

#define EXPORT_SYMBOL(sym)	__EXPORT_SYMBOL(sym)
//#define EXPORT_SYMBOL(sym)

#endif /* _SANDIX_EXPORT_H_ */
