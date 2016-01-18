/*
 *	arch/x86/boot/tools/build.c
 *
 *	Copyright (C) 2015 Yizhou Shan <shanyizhou@ict.ac.cn>
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

/*
 * This file builds a disk-image from three different files:
 *
 * - bootloader:	Simple bootloader for Sandix
 * - setup:		8086 machine code, sets up system parameters
 * - system:		80386 code for actual system
 *
 * It does some checking that all files are of the correct type, and writes
 * the result to the specified destination, padding to the right amount.
 * It also print some information to the stdout, error message to stderr.
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* x86 nop instruction */
#define NOP			0x90
#define ZERO			0

#define DEFAULT_MAJOR_ROOT	0
#define DEFAULT_MINOR_ROOT	0
#define DEFAULT_ROOT_DEV	(DEFAULT_MAJOR_ROOT << 8 | DEFAULT_MINOR_ROOT)

/* Confine setup beyond 62 sectors to avoid cross-cylinder */
#define SETUP_SECT_MIN		5
#define SETUP_SECT_MAX		62

unsigned char buf[SETUP_SECT_MAX*512];

static void die(const char *str, ...)
{
	va_list args;

	va_start(args, str);
	vfprintf(stderr, str, args);
	exit(1);
}

static void usage(void)
{
	die("Usage: build bootloader setup system image\n");
}

static inline void __put_unaligned_le16(unsigned short val, unsigned char *p)
{
	*p++ = val;
	*p++ = val >> 8;
}

static inline void put_unaligned_le16(unsigned short val, unsigned char *p)
{
	__put_unaligned_le16(val, p);
}

static inline void put_unaligned_le32(unsigned int val, unsigned char *p)
{
	__put_unaligned_le16(val, p);
	__put_unaligned_le16(val >> 16, p + 2);
}

static int get_color(void)
{
	char *color;

	/* exported in Makefile */
	color = getenv("KBUILD_COLOR");
	if (*color > '0' || *color <= '7')
		return *color - '0';
	else
		return 0;
}

int main(int argc, char **argv)
{
	FILE *fp_loader, *fp_setup, *fp_sys, *fp_image, *fp_image2;
	int len_loader, len_setup, len_sys;
	int sectors_setup, sectors_sys, sectors_image;
	int i, j, pad, sys_size;
	char c;

	if (argc != 5)
		usage();

	if ((fp_loader = fopen(argv[1], "r")) == NULL)
		die("Open %s failed\n", argv[1]);

	if ((fp_setup = fopen(argv[2], "r")) == NULL)
		die("Open %s failed\n", argv[2]);
	
	if ((fp_sys = fopen(argv[3], "r")) == NULL)
		die("Open %s failed\n", argv[3]);

	if ((fp_image = fopen(argv[4], "w")) == NULL)
		die("Create %s failed\n", argv[4]);
	
	if ((fp_image2 = fopen("arch/x86/boot/bzimage2", "w")) == NULL)
		die("Create b2 failed\n");
	fseek(fp_image, 0, SEEK_SET);
	fseek(fp_image2, 0, SEEK_SET);
	
	/* Read Bootloader */
	fseek(fp_loader, 0, SEEK_END);
	len_loader = ftell(fp_loader);
	fseek(fp_loader, 0, SEEK_SET);

	if (len_loader > (512 - 2))
		die("Bootloader is %d bytes which is too big\n", len_loader);
	
	/* Write Bootloader to image */
	for (i = 0; i < len_loader; i++) {
	 	c = fgetc(fp_loader);
		fputc(c, fp_image);
	}
	for (i = (512 - 2); i > len_loader; i--)
		fputc(NOP, fp_image);
	fputc(0x55, fp_image);
	fputc(0xAA, fp_image);

	/* Read Setup */
	len_setup = fread(buf, 1, sizeof(buf), fp_setup);
	if(ferror(fp_setup))
		die("Read error on `setup'\n");

	sectors_setup = (len_setup + 511) / 512;
	if (sectors_setup > SETUP_SECT_MAX)
		die("Setup is %d sectors which is too big\n", sectors_setup);
	if (sectors_setup < SETUP_SECT_MIN)
		sectors_setup = SETUP_SECT_MIN;
	j = sectors_setup * 512;
	memset(buf+len_setup, NOP, j-len_setup);

	/* Read System */
	fseek(fp_sys, 0, SEEK_END);
	len_sys = ftell(fp_sys);
	fseek(fp_sys, 0, SEEK_SET);

	/*
	 * Patch the header of setup
	 * The exact offset of each field if defined in header.S
	 * Those fields are supposed to work with boot loader.
	 */

	/* Set the number of 16-bytes paragraphs */
	sys_size = (len_sys + 15) / 16;
	put_unaligned_le32(sys_size, &buf[0x1f4]);
	
	/* Set the default root device  What is this? */
	put_unaligned_le16(DEFAULT_ROOT_DEV, &buf[508]);

	/* Set the sectors of setup */
	buf[0x1f1] = sectors_setup - 1;

	/* Write Setup to image */
	if (fwrite(buf, 1, j, fp_image) != j )
		die("Write Setup failed\n");

	/* Write System to image */
	for (i = 0; i < len_sys; i++) {
		c = fgetc(fp_sys);
		fputc(c, fp_image);
	}

	/* bzimag2 */
	if (fwrite(buf, 1, j, fp_image2) != j )
		die("Write Setup2 failed\n");
	fseek(fp_sys, 0, SEEK_SET);
	for (i = 0; i < len_sys; i++) {
		c = fgetc(fp_sys);
		fputc(c, fp_image2);
	}
	if ((pad = len_sys % 512)) {
		pad = 512 - pad;
		for (i = 0; i < pad; i++)
			fputc(NOP, fp_image2);
	}
	
	/*
	 * Yes, i choose to pad system to 512-bytes align, because
	 * loader have to load an entire sector even if you just want
	 * to read one byte. Load more then we need do no harm, but
	 * omit something is really harmful.
	 *
	 * Also, i choose to write sys_size which is 16-bytes align
	 * to setup header. So the loader knows the 'precise' size.
	 * Of course, this *16-bytes* comform with Linux boot protocol.
	 */

	/* Make it 512 bytes align */
	if ((pad = len_sys % 512)) {
		pad = 512 - pad;
		for (i = 0; i < pad; i++)
			fputc(NOP, fp_image);
	}

	/* Print Info */
	sectors_sys	= (len_sys + 511) / 512; 
	sectors_image	= 1 + sectors_setup + sectors_sys;
	
	printf("\033[3%dm", get_color());
	printf("[loader]  : %8d Bytes (pad to %4d sector)\n",  len_loader, 1);
	printf("[setup]   : %8d Bytes (pad to %4d sectors)\n", len_setup, sectors_setup);
	printf("[system]  : %8d Bytes (pad to %4d sectors)\n", len_sys, sectors_sys);
	printf("[bzImage] : %8d Bytes (Total: %4d sectors)\n", sectors_image*512, sectors_image);
	printf("\033[0m");

	fclose(fp_loader);
	fclose(fp_setup);
	fclose(fp_sys);
	fclose(fp_image);
	fclose(fp_image2);

	return 0;
}
