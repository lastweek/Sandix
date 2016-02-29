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

#include <sandix/bug.h>
#include <sandix/sort.h>
#include <sandix/page.h>
#include <sandix/types.h>
#include <sandix/kernel.h>
#include <sandix/string.h>
#include <sandix/printk.h>

#include <asm/e820.h>
#include <asm/setup.h>
#include <asm/pgtable.h>
#include <asm/bootparam.h>

/*
 * The e820 map is the map that gets modified e.g. with command line parameters
 * and that is also registered with modifications in the kernel resource tree
 * with the iomem_resource as parent.
 *
 * The e820_bios is directly saved after the BIOS-provided memory map is
 * copied. It doesn't get modified afterwards.
 */
struct e820map e820;
struct e820map e820_bios;

#ifdef CONFIG_X86_32
# ifdef CONFIG_X86_PAE
#  define MAX_ARCH_PFN	(1ULL << (36 - PAGE_SHIFT))
# else
#  define MAX_ARCH_PFN	(1ULL << (32 - PAGE_SHIFT))
# endif
#else
#  define MAX_ARCH_PFN	(MAXMEM >> PAGE_SHIFT)
#endif

/**
 *	e820_end_pfn	-	fine the highest PFN available
 *	@limit_pfn: threshold
 *
 *	Find the highest page frame number we have avaiable, within
 *	the given limit pfn.
 */
static unsigned long __init e820_end_pfn(unsigned long limit_pfn)
{
	int i;
	unsigned long last_pfn = 0;
	unsigned long max_arch_pfn = MAX_ARCH_PFN;

	for (i = 0; i < e820.nr_entries; i++) {
		struct e820entry *ei = &e820.map[i];
		unsigned long start_pfn;
		unsigned long end_pfn;

		/*
		 * Persistent memory is accounted as RAM for purposes
		 * of establishing max_pfn and mem_map.
		 */
		if (ei->type != E820_RAM && ei->type != E820_PRAM)
			continue;

		start_pfn = ei->addr >> PAGE_SHIFT;
		end_pfn = (ei->addr + ei->size) >> PAGE_SHIFT;

		if (start_pfn > limit_pfn)
			continue;

		if (end_pfn > limit_pfn) {
			last_pfn = limit_pfn;
			break;
		}

		if (end_pfn > last_pfn)
			last_pfn = end_pfn;
	}

	if (last_pfn > max_arch_pfn)
		last_pfn = max_arch_pfn;

	return last_pfn;
}

unsigned long __init e820_end_of_ram_pfn(void)
{
	return e820_end_pfn(MAX_ARCH_PFN);
}

/*
 * Low 1-GB memory
 */
unsigned long __init e820_end_of_low_ram_pfn(void)
{
	return e820_end_pfn(1UL << (32 - PAGE_SHIFT));
}

static const char *e820_type_to_string(u32 e820_type)
{
	switch (e820_type) {
	case E820_RESERVED_KERN:
	case E820_RAM:		return "System RAM";
	case E820_ACPI:		return "ACPI Tables";
	case E820_NVS:		return "ACPI Non-volatile Storage";
	case E820_UNUSABLE:	return "Unusable memory";
	case E820_PRAM:		return "Persistent Memory (legacy)";
	case E820_PMEM:		return "Persistent Memory";
	default:		return "Reserved";
	}
}

void __init e820_print_map(char *who)
{
	int i;

	for (i = 0; i < e820.nr_entries; i++) {
		printk(KERN_INFO "%s: [mem %#016Lx-%#016Lx] %s\n", who,
			e820.map[i].addr, (e820.map[i].addr + e820.map[i].size - 1),
			e820_type_to_string(e820.map[i].type));
	}
}

/*
 * Sanitize the BIOS e820 map.
 *
 * Some e820 responses include overlapping entries. The following
 * replaces the original e820 map with a new one, removing overlaps,
 * and resolving conflicting memory types in favor of highest
 * numbered type.
 *
 * The input parameter biosmap points to an array of 'struct
 * e820entry' which on entry has elements in the range [0, *pnr_map)
 * valid, and which has space for up to max_nr_map entries.
 * On return, the resulting sanitized e820 map entries will be in
 * overwritten in the same location, starting at biosmap.
 *
 * The integer pointed to by pnr_map must be valid on entry (the
 * current number of valid entries located at biosmap). If the
 * sanitizing succeeds the *pnr_map will be updated with the new
 * number of valid entries (something no more than max_nr_map).
 *
 * The return value from sanitize_e820_map() is zero if it
 * successfully 'sanitized' the map entries passed in, and is -1
 * if it did nothing, which can happen if either of (1) it was
 * only passed one map entry, or (2) any of the input map entries
 * were invalid (start + size < start, meaning that the size was
 * so big the described memory range wrapped around through zero.)
 *
 *	Visually we're performing the following
 *	(1,2,3,4 = memory types)...
 *
 *	Sample memory map (w/overlaps):
 *	   ____22__________________
 *	   ______________________4_
 *	   ____1111________________
 *	   _44_____________________
 *	   11111111________________
 *	   ____________________33__
 *	   ___________44___________
 *	   __________33333_________
 *	   ______________22________
 *	   ___________________2222_
 *	   _________111111111______
 *	   _____________________11_
 *	   _________________4______
 *
 *	Sanitized equivalent (no overlap):
 *	   1_______________________
 *	   _44_____________________
 *	   ___1____________________
 *	   ____22__________________
 *	   ______11________________
 *	   _________1______________
 *	   __________3_____________
 *	   ___________44___________
 *	   _____________33_________
 *	   _______________2________
 *	   ________________1_______
 *	   _________________4______
 *	   ___________________2____
 *	   ____________________33__
 *	   ______________________4_
 */

struct change_member {
	struct e820entry *pbios; /* pointer to original bios entry */
	unsigned long long addr; /* address for this change point */
};

static int __init cpcompare(const void *a, const void *b)
{
	struct change_member * const *app = a, * const *bpp = b;
	const struct change_member *ap = *app, *bp = *bpp;

	/*
	 * Inputs are pointers to two elements of change_point[].  If their
	 * addresses are unequal, their difference dominates.  If the addresses
	 * are equal, then consider one that represents the end of its region
	 * to be greater than one that does not.
	 */
	if (ap->addr != bp->addr)
		return ap->addr > bp->addr ? 1 : -1;

	return (ap->addr != ap->pbios->addr) - (bp->addr != bp->pbios->addr);
}

int __init sanitize_e820_map(struct e820entry *biosmap, int max_nr_map,
			     u32 *pnr_map)
{
	static struct change_member change_point_list[2*E820MAX] __initdata;
	static struct change_member *change_point[2*E820MAX] __initdata;
	static struct e820entry *overlap_list[E820MAX] __initdata;
	static struct e820entry new_bios[E820MAX] __initdata;
	unsigned long current_type, last_type;
	unsigned long long last_addr;
	int chgidx;
	int overlap_entries;
	int new_bios_entry;
	int old_nr, new_nr, chg_nr;
	int i;

	/* if there's only one memory region, don't bother */
	if (*pnr_map < 2)
		return -1;

	old_nr = *pnr_map;
	BUG_ON(old_nr > max_nr_map);

	/* bail out if we find any unreasonable addresses in bios map */
	for (i = 0; i < old_nr; i++)
		if (biosmap[i].addr + biosmap[i].size < biosmap[i].addr)
			return -1;

	/* create pointers for initial change-point information (for sorting) */
	for (i = 0; i < 2 * old_nr; i++)
		change_point[i] = &change_point_list[i];

	/* record all known change-points (starting and ending addresses),
	   omitting those that are for empty memory regions */
	chgidx = 0;
	for (i = 0; i < old_nr; i++)	{
		if (biosmap[i].size != 0) {
			change_point[chgidx]->addr = biosmap[i].addr;
			change_point[chgidx++]->pbios = &biosmap[i];
			change_point[chgidx]->addr = biosmap[i].addr +
				biosmap[i].size;
			change_point[chgidx++]->pbios = &biosmap[i];
		}
	}
	chg_nr = chgidx;

	/* sort change-point list by memory addresses (low -> high) */
	sort(change_point, chg_nr, sizeof *change_point, cpcompare, NULL);

	/* create a new bios memory map, removing overlaps */
	overlap_entries = 0;	 /* number of entries in the overlap table */
	new_bios_entry = 0;	 /* index for creating new bios map entries */
	last_type = 0;		 /* start with undefined memory type */
	last_addr = 0;		 /* start with 0 as last starting address */

	/* loop through change-points, determining affect on the new bios map */
	for (chgidx = 0; chgidx < chg_nr; chgidx++) {
		/* keep track of all overlapping bios entries */
		if (change_point[chgidx]->addr ==
		    change_point[chgidx]->pbios->addr) {
			/*
			 * add map entry to overlap list (> 1 entry
			 * implies an overlap)
			 */
			overlap_list[overlap_entries++] =
				change_point[chgidx]->pbios;
		} else {
			/*
			 * remove entry from list (order independent,
			 * so swap with last)
			 */
			for (i = 0; i < overlap_entries; i++) {
				if (overlap_list[i] ==
				    change_point[chgidx]->pbios)
					overlap_list[i] =
						overlap_list[overlap_entries-1];
			}
			overlap_entries--;
		}
		/*
		 * if there are overlapping entries, decide which
		 * "type" to use (larger value takes precedence --
		 * 1=usable, 2,3,4,4+=unusable)
		 */
		current_type = 0;
		for (i = 0; i < overlap_entries; i++)
			if (overlap_list[i]->type > current_type)
				current_type = overlap_list[i]->type;
		/*
		 * continue building up new bios map based on this
		 * information
		 */
		if (current_type != last_type || current_type == E820_PRAM) {
			if (last_type != 0)	 {
				new_bios[new_bios_entry].size =
					change_point[chgidx]->addr - last_addr;
				/*
				 * move forward only if the new size
				 * was non-zero
				 */
				if (new_bios[new_bios_entry].size != 0)
					/*
					 * no more space left for new
					 * bios entries ?
					 */
					if (++new_bios_entry >= max_nr_map)
						break;
			}
			if (current_type != 0)	{
				new_bios[new_bios_entry].addr =
					change_point[chgidx]->addr;
				new_bios[new_bios_entry].type = current_type;
				last_addr = change_point[chgidx]->addr;
			}
			last_type = current_type;
		}
	}
	/* retain count for new bios entries */
	new_nr = new_bios_entry;

	/* copy new bios mapping into original location */
	memcpy(biosmap, new_bios, new_nr * sizeof(struct e820entry));
	*pnr_map = new_nr;

	return 0;
}

/* Add a memory region to the kernel e820 map. */
static void __init __e820_add_region(struct e820map *e820x, u64 start,
				     u64 size, u32 type)
{
	int x = e820x->nr_entries;

	if (x >= ARRAY_SIZE(e820x->map)) {
		printk(KERN_ERR "e820: too many entries; ignoring [mem %#016Lx-%#016Lx]\n",
			start, (start + size - 1));
		return;
	}

	e820x->map[x].addr = start;
	e820x->map[x].size = size;
	e820x->map[x].type = type;
	e820x->nr_entries++;
}

void __init e820_add_region(u64 start, u64 size, u32 type)
{
	__e820_add_region(&e820, start, size, type);
}

static int __init __append_e820_map(struct e820entry *biosmap, int nr_map)
{
	while (nr_map) {
		u64 start = biosmap->addr;
		u64 size  = biosmap->size;
		u64 end   = start + size;
		u32 type  = biosmap->type;

		/* Overflow in 64 bits? Ignore the memory map. */
		if (start > end)
			return -1;

		e820_add_region(start, size, type);

		biosmap++;
		nr_map--;
	}
	return 0;
}

static int __init append_e820_map(struct e820entry *biosmap, int nr_map)
{
	/* Only one memory region (or negative)? Ignore it */
	if (nr_map < 2)
		return -1;

	return __append_e820_map(biosmap, nr_map);
}

/*
 * Sanitize the e820 table from BIOS, and then copy it
 * a safe place: e820_bios.
 *
 * If we have a empty e820 table, we do not fake one, just panic.
 */
void __init setup_memory_map(void)
{
	sanitize_e820_map(boot_params.e820_map,
			  ARRAY_SIZE(boot_params.e820_map),
			  &boot_params.e820_nr_entries);

	if (append_e820_map(boot_params.e820_map, boot_params.e820_nr_entries) < 0)
		panic("e820 error");
	
	memcpy(&e820_bios, &e820, sizeof(struct e820map));

	printk(KERN_INFO "e820: BIOS-provided physical RAM map:\n");
	e820_print_map("BIOS-e820");
}
