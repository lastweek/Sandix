#ifndef _ASM_X86_BOOTPARAM_H_
#define _ASM_X86_BOOTPARAM_H_

#include <sandix/screen_info.h>
#include <asm/e820.h>

/* setup_header in header.S */
struct setup_header {
	unsigned int	header_version;
	unsigned int	code32_start;
	unsigned int	video_mode;
	unsigned int	rm_kernel_sects;
	unsigned int	pm_kernel_sects;
	unsigned char	header_signature[8];
} __attribute__((packed));


/* System information fetched from BIOS in boot time */
/* The structure is padded to a page size, 4096 Bytes */
struct boot_params {
	struct setup_header	hdr;				/* 0 - 27, 28 bytes */
	struct screen_info	screen_info;		/* 28 - 39, 12 bytes */
	struct e820entry	e820_map[E820MAX];	/* 40 - 2599, 2560 bytes*/
	unsigned int	e820_entries_nr;		/* 2600 - 2603, 4 bytes */
	unsigned char	kbd_status;
	unsigned char	pad1[3];				/* 2604 - 2607, 4 bytes */
	unsigned char	pad2[1488];				/* 2608 - 4095, padding*/			
} __attribute__((packed));

#define BOOT_PARAMS_SIZE	4096

#endif /* _ASM_X86_BOOTPARAM_H_ */
