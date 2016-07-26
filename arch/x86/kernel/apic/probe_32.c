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

#include <asm/apic.h>

#include <sandix/export.h>

static int default_apic_probe(void)
{
	return 1;
}

static struct apic apic_default = {
	.name		= "default",
	.probe		= default_apic_probe,
};

apic_driver(apic_default);

/*
 * Pointer to the local APIC driver in use on this system (there's
 * always just one such driver in use - the kernel decides via an
 * early probing process which one it picks - and then sticks to it):
 */
struct apic *apic = &apic_default;
EXPORT_SYMBOL(apic);

void generic_apic_probe(void)
{
	struct apic **driver;

	for (driver = __apicdrivers; driver < __apicdrivers_end; driver++) {
		if ((*driver)->probe()) {
			apic = *driver;
			break;
		}
	}

	if (driver == __apicdrivers_end)
		panic("Can not find an APIC driver");

	pr_info("Using APIC driver: %s\n", apic->name);
}
