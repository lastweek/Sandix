/*
 *	Copyright (C) 2016 Yizhou Shan <shan13@purdue.edu>
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

#include <sandix/mm.h>
#include <sandix/bootmem.h>

unsigned long max_low_pfn;
unsigned long min_low_pfn;
unsigned long max_pfn;

/*
 * This is the bootmem.c in Linux, which uses bitmap to allocate pages in the
 * very early initialization stage. It should be noted that Linux's implementation
 * of bitmap allocation is more efficient than my DSNVM one...
 *
 * Anyway, I did not plan to write this code, just use bootmem_memblk to serve
 * the bootmem requests, and this is the default setting of Linux x86.
 */
