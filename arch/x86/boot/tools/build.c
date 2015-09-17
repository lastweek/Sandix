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

/*
 * build.c: Concatenate three images.
 *
 * The three images are:
 *	--> Bootloader.bin
 *	--> Setup.bin
 *	--> vmSandix.bin
 */

#include <stdio.h>
#include <stdlib.h>

#define NOP		0x90
#define SECTOR_SIZE	512
#define MAGIC_511	0x55
#define MAGIC_512	0xAA

/* Current di*/
const char *bootloader	= "arch/x86/boot/bootloader.bin";
const char *setup	= "arch/x86/boot/setup.bin";
const char *kernel	= "arch/x86/boot/vmSandix.bin";
const char *bzImage	= "arch/x86/boot/bzImage";

int main(int argc, char **argv)
{
	FILE *fp_bl, *fp_si, *fp_ki;	// Input file handles
	FILE *fp_out;			// Output file handle
	int len_bl, len_si, len_ki;	// Input files's length
	int sectors_header;		// Sectors of header
	int sectors_image;		// Sectors of image
	int sectors_bzimage;		// Sectors of bzimage
	int i, pad;
	char c;

	if ((fp_bl = fopen(bootloader, "r+")) == NULL) {
		printf("Open %s failed\n", bootloader);
		exit(-1);
	}

	if ((fp_si = fopen(setup, "r+")) == NULL) {
		printf("Open %s failed\n", setup);
		exit(-1);
	}
	
	if ((fp_ki = fopen(kernel, "r+")) == NULL) {
		printf("Open %s failed\n", kernel);
		exit(-1);
	}

	if ((fp_out = fopen(bzImage, "w+")) == NULL) {
		printf("Create %s failed\n", bzImage);
		exit(-1);
	}
	fseek(fp_out, 0, SEEK_SET);

	
	/**
	 * Step1: Handle bootloader
	 **/

	fseek(fp_bl, 0, SEEK_END);
	len_bl = ftell(fp_bl);
	fseek(fp_bl, 0, SEEK_SET);
	if (len_bl > (SECTOR_SIZE - 2)) {
		printf("bootloader is %d bytes which is too large\n", len_bl);
		exit(-1);
	}

	for (i = 0; i < len_bl; i++) {
	 	c = fgetc(fp_bl);
		fputc(c, fp_out);
	}

	for (i = (SECTOR_SIZE-2); i > len_bl; i--) {
		fputc(NOP, fp_out);
	}
	
	// MBR MAGIC
	fputc(MAGIC_511, fp_out);
	fputc(MAGIC_512, fp_out);

	/**
	 * Step2: Handle Real-Mode Kernel Image
	 **/

	fseek(fp_si, 0, SEEK_END);
	len_si = ftell(fp_si);
	fseek(fp_si, 0, SEEK_SET);
	for (i = 0; i < len_si; i++) {
		c = fgetc(fp_si);
		fputc(c, fp_out);
	}
	
	// make it 512 byte alignment.
	if ((pad = len_si % SECTOR_SIZE)) {
		pad = SECTOR_SIZE - pad;
		for (i = 0; i < pad; i++)
			fputc(NOP, fp_out);
	}

	/**
	 * Step3: Handle Protected-Mode Kernel Image
	 **/

	 fseek(fp_ki, 0, SEEK_END);
	 len_ki = ftell(fp_ki);
	 fseek(fp_ki, 0, SEEK_SET);
	 for (i = 0; i < len_ki; i++) {
	 	c = fgetc(fp_ki);
		fputc(c, fp_out);
	 }
	
	// make it 512 byte alignment.
	if ((pad = len_ki % SECTOR_SIZE)) {
		pad = SECTOR_SIZE - pad;
		for (i = 0; i < pad; i++)
			fputc(NOP, fp_out);
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
	printf("Remember changing the sector number in boot loader!\n");

	fclose(fp_bl);
	fclose(fp_si);
	fclose(fp_ki);
	fclose(fp_out);

	return 0;
}
