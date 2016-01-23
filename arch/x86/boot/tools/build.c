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

/*
 * This file builds two disk-image from three different files:
 *
 * - bootloader:	Simple bootloader for Sandix
 * - setup:		8086 machine code, sets up system parameters
 * - system:		80386 code for actual system
 *
 * It does some checking that all files are of the correct type, and writes
 * the result to the specified destination, padding to the right amount.
 * It also print some information to the stdout, error message to stderr.
 *
 * The output disk-image:
 * - bzImage		kernel image that could be loaded by GRUB or LILO
 * - bzImage.iso	bootloader + bzImage, which could be loaded by BIOS
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#define DEFAULT_MAJOR_ROOT	0
#define DEFAULT_MINOR_ROOT	0
#define DEFAULT_ROOT_DEV	(DEFAULT_MAJOR_ROOT << 8 | DEFAULT_MINOR_ROOT)

/* Confine setup beyond 62 sectors to avoid cross-cylinder */
#define SETUP_SECT_MIN		5
#define SETUP_SECT_MAX		62

static void die(const char *str, ...)
{
	va_list args;

	va_start(args, str);
	vfprintf(stderr, str, args);
	fputc('\n', stderr);
	exit(1);
}

static inline unsigned short get_le16(const unsigned char *p)
{
	return p[0] | p[1] << 8;
}

static inline unsigned int get_le32(const unsigned char *p)
{
	return p[0] | p[1] << 8 | p[2] << 16 | p[3] << 24;
}

static inline void put_le16(unsigned short val, unsigned char *p)
{
	*p++ = val;
	*p++ = val >> 8;
}

static inline void put_le32(unsigned int val, unsigned char *p)
{
	put_le16(val, p);
	put_le16(val >> 16, p + 2);
}

static int get_color(void)
{
	char *color;

	/* exported in top Makefile */
	color = getenv("KBUILD_COLOR");
	if (*color > '0' || *color <= '7')
		return *color - '0';
	else
		return 0;
}

static void usage(void)
{
	die("Usage: build bootloader setup system image\n");
}

int main(int argc, char **argv)
{
	FILE *file, *bzImage, *iso;
	char iso_path[255];
	unsigned char loader[512];
	unsigned char setup[SETUP_SECT_MAX * 512];
	unsigned int i, c, sz, setup_sectors;

	int fd;
	struct stat sb;
	void *kernel;
	unsigned int sys_size;

	if (argc != 5)
		usage();

	/*
	 * OPEN bzImage
	 */
	bzImage = fopen(argv[4], "w");
	if (!bzImage)
		die("Unable to write [%s]", argv[4]);

	/*
	 * OPEN bzImage.iso
	 */
	strcpy(iso_path, argv[4]);
	strcat(iso_path, ".iso");
	iso = fopen(iso_path, "w");
	if (!iso)
		die("Unable to write [%s]", iso_path);

	/*
	 * OPEN kernel file
	 */
	fd = open(argv[3], O_RDONLY);
	if (fd < 0)
		die("Unable to open [%s]", argv[3]);
	if (fstat(fd, &sb))
		die("Unable to stat [%s]", argv[3]);

	/* number of 16-bytes paragraphs */
	sz = sb.st_size;
	sys_size = (sz + 15) / 16;
	printf("\033[3%dmSystem is %d KB\n\033[0m",
	       get_color(), (sz + 1023) / 1024);

	kernel = mmap(NULL, sz, PROT_READ, MAP_SHARED, fd, 0);
	if (kernel == MAP_FAILED)
		die("Unable to mmap [%s]", argv[3]);

	/*
	 * OPEN bootloader
	 */
	file = fopen(argv[1], "r");
	if (!file)
		die("Unable to open [%s]", argv[1]);
	c = fread(loader, 1, sizeof(loader), file);
	if (ferror(file))
		die("Read error on loader");
	if (c > 510)
		die("The bootloader is too big");
	fclose(file);

	/*
	 * PAD bootloader buffer
	 */
	memset(loader + c, 0, 512 - c);
	put_le16(0xAA55, &loader[510]);

	/*
	 * COPY setup code
	 */
	file = fopen(argv[2], "r");
	if (!file)
		die("Unable to open [%s]", argv[2]);

	c = fread(setup, 1, sizeof(setup), file);
	if (ferror(file))
		die("Read error on setup");
	if (get_le16(&setup[510]) != 0xAA55)
		die("The setup hasn't got boot flag (0xAA55)");
	fclose(file);

	/*
	 * PAD setup buffer
	 */
	setup_sectors = (c + 511) / 512;
	if (setup_sectors < SETUP_SECT_MIN)
		setup_sectors = SETUP_SECT_MIN;
	i = setup_sectors * 512;
	memset(setup + c, 0, i - c);
	printf("\033[3%dmSetup is %d bytes (padded to %d bytes).\n\033[0m",
	       get_color(), c, i);

	/* Sector 0, Offset 0x1f1, setup_sects */
	setup[0x1f1] = setup_sectors - 1;

	/* Sector 0, Offset 0x1f4, syssize */
	put_le32(sys_size, &setup[0x1f4]);

	/* Sector 0, Offset 0x1fc, root_dev */
	put_le16(DEFAULT_ROOT_DEV, &setup[0x1fc]);

	/*
	 * WRITE to image
	 */

	/* bzImage */
	if (fwrite(setup, 1, i, bzImage) != i)
		die("Writing setup failed 1");
	if (fwrite(kernel, 1, sz, bzImage) != sz)
		die("Writing kernel failed 1");

	c = sys_size * 16 - sz;
	while (c--)
		if (fwrite("\0", 1, 1, bzImage) != 1)
			die("Writing padding failed 1");

	/* bzImage.iso */
	if (fwrite(loader, 1, 512, iso) != 512)
		die("Writing bootloader failed 2");
	if (fwrite(setup, 1, i, iso) != i)
		die("Writing setup failed 2");
	if (fwrite(kernel, 1, sz, iso) != sz)
		die("Writing kernel failed 2");

	c = sys_size * 16 - sz;
	while (c--)
		if (fwrite("\0", 1, 1, iso) != 1)
			die("Writing padding failed 2");

	/*
	 * Clean up
	 */
	sync();
	fclose(bzImage);
	fclose(iso);
	close(fd);

	return 0;
}
