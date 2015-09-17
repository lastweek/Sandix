/*
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
 *
 */

#include <stdio.h>
#include <stdlib.h>

#define NOP		0x90
#define SECTOR_SIZE	512
#define MAGIC_511	0x55
#define MAGIC_512	0xAA

static void usage(void)
{
	printf("Usage: build bootloader setup system image\n");
	exit(-1);
}

int main(int argc, char **argv)
{
	FILE *fp_loader, *fp_setup, *fp_sys;	// Input file handles
	FILE *fp_image;				// Output file handle
	int len_bl, len_si, len_ki;		// Input files's length
	int sectors_header;			// Sectors of header
	int sectors_image;			// Sectors of image
	int sectors_bzimage;			// Sectors of bzimage
	int i, pad;
	char c;

	if (argc != 5)
		usage();

	if ((fp_loader = fopen(argv[1], "r+")) == NULL) {
		printf("Open %s failed\n", argv[1]);
		exit(-1);
	}

	if ((fp_setup = fopen(argv[2], "r+")) == NULL) {
		printf("Open %s failed\n", argv[2]);
		exit(-1);
	}
	
	if ((fp_sys = fopen(argv[3], "r+")) == NULL) {
		printf("Open %s failed\n", argv[3]);
		exit(-1);
	}

	if ((fp_image = fopen(argv[4], "w+")) == NULL) {
		printf("Create %s failed\n", argv[4]);
		exit(-1);
	}
	fseek(fp_image, 0, SEEK_SET);

	
	/**
	 * Step1: Handle bootloader
	 **/

	fseek(fp_loader, 0, SEEK_END);
	len_bl = ftell(fp_loader);
	fseek(fp_loader, 0, SEEK_SET);
	if (len_bl > (SECTOR_SIZE - 2)) {
		printf("bootloader is %d bytes which is too large\n", len_bl);
		exit(-1);
	}

	for (i = 0; i < len_bl; i++) {
	 	c = fgetc(fp_loader);
		fputc(c, fp_image);
	}

	for (i = (SECTOR_SIZE-2); i > len_bl; i--) {
		fputc(NOP, fp_image);
	}
	
	// MBR MAGIC
	fputc(MAGIC_511, fp_image);
	fputc(MAGIC_512, fp_image);

	/**
	 * Step2: Handle Real-Mode Kernel Image
	 **/

	fseek(fp_setup, 0, SEEK_END);
	len_si = ftell(fp_setup);
	fseek(fp_setup, 0, SEEK_SET);
	for (i = 0; i < len_si; i++) {
		c = fgetc(fp_setup);
		fputc(c, fp_image);
	}
	
	// make it 512 byte alignment.
	if ((pad = len_si % SECTOR_SIZE)) {
		pad = SECTOR_SIZE - pad;
		for (i = 0; i < pad; i++)
			fputc(NOP, fp_image);
	}

	/**
	 * Step3: Handle Protected-Mode Kernel Image
	 **/

	 fseek(fp_sys, 0, SEEK_END);
	 len_ki = ftell(fp_sys);
	 fseek(fp_sys, 0, SEEK_SET);
	 for (i = 0; i < len_ki; i++) {
	 	c = fgetc(fp_sys);
		fputc(c, fp_image);
	 }
	
	// make it 512 byte alignment.
	if ((pad = len_ki % SECTOR_SIZE)) {
		pad = SECTOR_SIZE - pad;
		for (i = 0; i < pad; i++)
			fputc(NOP, fp_image);
	}

	/**
	 * Step4: Print some infomation.
	 **/
	sectors_header    = (len_si % SECTOR_SIZE)? (len_si/SECTOR_SIZE+1): len_si/SECTOR_SIZE;
	sectors_image     = (len_ki % SECTOR_SIZE)? (len_ki/SECTOR_SIZE+1): len_ki/SECTOR_SIZE;
	sectors_bzimage   = sectors_header + sectors_image + 1;
	
	printf("\n");
	printf("[bootloader]   : %-10d bytes (%-5d sector)\n", len_bl, 1);
	printf("[setup.bin]    : %-10d bytes (%-5d sectors)\n", len_si, sectors_header);
	printf("[vmSandix.bin] : %-10d bytes (%-5d sectors)\n", len_ki, sectors_image);
	printf("[bzImage]      : %-10d bytes (%-5d sectors)\n", sectors_bzimage*SECTOR_SIZE, sectors_bzimage);

	fclose(fp_loader);
	fclose(fp_setup);
	fclose(fp_sys);
	fclose(fp_image);

	return 0;
}
