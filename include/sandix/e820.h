#ifndef _SANDIX_E820_H
#define _SANDIX_E820_H

#define E820MAP			0x2d0	/* our map */
#define E820MAX			128		/* number of entries in E820MAP */
#define E820NR			0x1e8	/* # entries in E820MAP */

#define E820_RAM		1		/* Available RAM */
#define E820_RESERVED	2		/* Reserved */
#define E820_ACPI		3
#define E820_NVS		4
#define E820_UNUSABLE	5

#include <sandix/types.h>

struct e820entry {
	__u32 base_low;	/* start of memory segment */
	__u32 base_high;
	__u32 len_low;	/* length of memory segment */
	__u32 len_high;
	__u32 type;		/* type of memory segment */
} __attribute__((packed));

struct e820_list {
	__u32 nr;
	struct e820entry list[E820MAX];
};

#define ISA_START_ADDRESS	0xa0000
#define ISA_END_ADDRESS		0x100000

#define BIOS_BEGIN		0x000a0000
#define BIOS_END		0x00100000

#define BIOS_ROM_BASE		0xffe00000
#define BIOS_ROM_END		0xffffffff

#endif /* _SANDIX_E820_H */
