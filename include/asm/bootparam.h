#ifndef _ASM_X86_BOOTPARAM_H
#define _ASM_X86_BOOTPARAM_H

#include <sandix/types.h>
#include <sandix/screen_info.h>
#include <asm/e820.h>

/* setup_header in header.S */
struct setup_header {
	__u32	header_version;
	__u32	code32_start;
	__u32	video_mode;
	__u32	rm_kernel_sects;
	__u32	pm_kernel_sects;
	__u8	header_signature[8];
} __attribute__((packed));


/* System information fetched from BIOS in boot time */
struct boot_params {
	struct	setup_header	hdr;
	struct	screen_info		screen_info;
	struct	e820entry		e820_map[E820MAX];
	__u32	e820_entries_nr;
	__u8	kbd_status;
	__u8	pad[3];
} __attribute__((packed));

#endif /* _ASM_X86_BOOTPARAM_H */
