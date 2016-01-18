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

#ifndef _ASM_X86_PCI_EARLY_H_
#define _ASM_X86_PCI_EARLY_H_

#include <sandix/types.h>

/*
 * Direct PCI access. This is used for PCI accesses in
 * early boot before the PCI subsystem works.
 */

extern u32 read_pci_config(u8 bus, u8 slot, u8 func, u8 offset);
extern u8 read_pci_config_byte(u8 bus, u8 slot, u8 func, u8 offset);
extern u16 read_pci_config_16(u8 bus, u8 slot, u8 func, u8 offset);
extern void write_pci_config(u8 bus, u8 slot, u8 func, u8 offset, u32 val);
extern void write_pci_config_byte(u8 bus, u8 slot, u8 func, u8 offset, u8 val);
extern void write_pci_config_16(u8 bus, u8 slot, u8 func, u8 offset, u16 val);
extern void early_dump_pci_device(u8 bus, u8 slot, u8 func);
extern void early_dump_pci_devices(void);

#endif /* _ASM_X86_PCI_EARLY_H_ */
