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

#include <sandix/io.h>
#include <sandix/compiler.h>

/*

Different from Linux's implementation, I choose to use a simpler way to describe
those legacy ROM areas. The following descriptions are copied from the datasheet
of Intel. Anyway, I guess I won't use this 1 MB, those structures are just used
to document themselves.

---

The memory address range from 0 to 1 MB is known as Legacy Address. This area
is divided into the following address regions:
 * 0 - 640 KB		DOS Area
 * 640 - 768 KB		Legacy Video Buffer Area
 * 768 - 896 KB		Expansion Area
 * 896 - 960 KB		Extended System BIOS Area
 * 960 KB - 1 MB	System BIOS Area

The area between 768 KB – 1 MB is also collectively referred to
as PAM (Programmable Address Memory). All accesses to the DOS and PAM ranges
from any device are sent to DRAM. However, access to the legacy video buffer
area is treated differently.

---

Programmable Attribute Map (PAM) (C_0000h – F_FFFFh):

PAM is a legacy BIOS ROM area in MMIO. It is overlaid with DRAM and used as a
faster ROM storage area. It has a fixed base address (000C_0000h) and fix size
of 256 KB. The 13 sections from 768 KB to 1 MB comprise what is also known as
the PAM Memory Area. Each section has Read enable and Write enable attributes.

The PAM registers are mapped in Device 0 configuration space.
 * ISA Expansion Area (C_0000h – D_FFFFh)
 * Extended System BIOS Area (E_0000h – E_FFFFh)
 * System BIOS Area (F_0000h – F_FFFFh)

The processor decodes the Core request, then routes to the appropriate destination (DRAM or DMI).

Snooped accesses from PCI Express or DMI to this region are snooped on processor Caches.

Non-snooped accesses from PCI Express or DMI to this region are always sent to DRAM.
*/

static struct resource system_rom_resource = {
	.name	= "System BIOS (Upper)",
	.end	= 0xfffff,
	.start	= 0xf0000,
	.flags	= IORESOURCE_BUSY | IORESOURCE_READONLY | IORESOURCE_MEM
};

static struct resource extension_rom_resource = {
	.name	= "Extended System BIOS (Lower)",
	.end	= 0xeffff,
	.start	= 0xe0000,
	.flags	= IORESOURCE_BUSY | IORESOURCE_READONLY | IORESOURCE_MEM
};

static struct resource expansion_rom_resource = {
	.name	= "Expansion Area",
	.end	= 0xdffff,
	.start	= 0xc0000,
	.flags	= IORESOURCE_BUSY | IORESOURCE_READONLY | IORESOURCE_MEM
};

static struct resource video_rom_resource = {
	.name 	= "Video ROM",
	.end	= 0xbffff,
	.start	= 0xa0000,
	.flags	= IORESOURCE_BUSY | IORESOURCE_READONLY | IORESOURCE_MEM
};

static struct resource dos_rom_resource = {
	.name	= "DOS Area",
	.end	= 0x9ffff,
	.start	= 0x00000,
	.flags	= IORESOURCE_BUSY | IORESOURCE_READONLY | IORESOURCE_MEM
};

static struct resource legacy_address_rom_resource = {
	.name	= "Legacy Address",
	.end	= 0x100000,
	.start	= 0x00000,
	.flags	= IORESOURCE_BUSY | IORESOURCE_READONLY | IORESOURCE_MEM
};

void __init probe_roms(void)
{
	request_resource(&iomem_resource, &legacy_address_rom_resource);
	request_resource(&iomem_resource, &dos_rom_resource);
	request_resource(&iomem_resource, &video_rom_resource);
	request_resource(&iomem_resource, &expansion_rom_resource);
	request_resource(&iomem_resource, &extension_rom_resource);
	request_resource(&iomem_resource, &system_rom_resource);
}
