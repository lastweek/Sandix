#ifndef _ASM_E820_H_
#define _ASM_E820_H_

/* e820 memory segment type */
#define E820_RAM_TYPE		1		/* Available RAM */
#define E820_RESERVED_TYPE	2		/* Reserved */
#define E820_ACPI_TYPE		3
#define E820_NVS_TYPE		4
#define E820_UNUSABLE_TYPE	5

/* Conventional segment start address */
#define ISA_START_ADDRESS	0xa0000
#define ISA_END_ADDRESS		0x100000
#define BIOS_BEGIN			0x000a0000
#define BIOS_END			0x00100000
#define BIOS_ROM_BASE		0xffe00000
#define BIOS_ROM_END		0xffffffff

/* Max e820 entries */
#define E820MAX				128

#ifndef __ASSEMBLY__

/* e820entry structure, returned by BIOS */
struct e820entry {
	unsigned long long	addr;	/* start of memory segment */
	unsigned long long	size;	/* size of memory segment */
	unsigned int		type;	/* type of memory segment */
} __attribute__((packed));

struct e820map {
	unsigned int nr_map;
	struct e820entry map[E820MAX];
}__attribute__((packed));

#endif /* __ASSEMBLY__ */

#endif /* _ASM_E820_H_ */
