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

#include <sandix/io.h>
#include <sandix/errno.h>
#include <sandix/types.h>
#include <sandix/export.h>
#include <sandix/compiler.h>
#include <sandix/spinlock.h>

struct resource ioport_resource  = {
	.name	= "PCI IO",
	.start	= 0,
	.end	= 0,
	.flags	= IORESOURCE_IO
};
EXPORT_SYMBOL(ioport_resource);

struct resource iomem_resource = {
	.name	= "PCI memory",
	.start	= 0,
	.end	= -1,
	.flags	= IORESOURCE_MEM
};
EXPORT_SYMBOL(iomem_resource);

/* a read/write lock is more efficient */
static DEFINE_SPINLOCK(resource_lock);

int release_resource(struct resource *old)
{
	return 0;
}
EXPORT_SYMBOL(release_resource);

int request_resource(struct resource *root, struct resource *new)
{
	return 0;
}
EXPORT_SYMBOL(request_resource);