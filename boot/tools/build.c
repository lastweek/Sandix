/*
 * 2015/03/22 Created by Shan Yizhou.
 * 
 * build.c: Concatenate three images.
 *
 * The three images are:
 * 1. Bootloader object
 * 2. 16-bit setup image
 * 3. 32-bit kernel image
 */

#include <stdio.h>
#include <stdlib.h>

#define NOP				0x90
#define SECTOR_SIZE		512
#define MAGIC_510		0x55
#define MAGIC_511		0xAA

/* Filenames */
const char *bootloader	= "bootsect";
const char *setup		= "header";
const char *kernel		= "image";
const char *bzimage		= "bzimage";

void err(char *s) {
	printf("%s", s);
	exit(1);
}

/* just need output image size. */
int main(int argc, char **argv) {
	FILE *fp_bl, *fp_si, *fp_ki;	// Input file handles
	FILE *fp_out;					// Output file handle
	int len_bl, len_si, len_ki;		// Input files's length
	int image_size;					// 1KB, 2KB...
	int sectors_header;				// Sectors of header
	int lower_bound_of_sp;			// The lower bound of stack point, %sp
	int i;
	char c;

	if (argc > 2)
		err("Too many arguments\n");

	if ((image_size = atoi(argv[argc-1])) == 0)
		err("Please specify a valid final image size.\n");

	if ((fp_bl = fopen(bootloader, "r+")) == NULL)
		err("Open [bootsect] failed\n");

	if ((fp_si = fopen(setup, "r+")) == NULL)
		err("Open [header] failed\n");

	/* Step1: Padding the hole in bootloader. */
	fseek(fp_bl, -1, SEEK_END);// Skip EOF in the end.
	if ((len_bl = ftell(fp_bl)) > (SECTOR_SIZE - 2))
		err("[bootsect] is too large to form MBR\n");
	
	//Padding the hole with NOP
	while (len_bl < (SECTOR_SIZE - 2)) {
		fputc(NOP, fp_bl);
		len_bl++;
	}
	fputc(MAGIC_510, fp_bl);
	fputc(MAGIC_511, fp_bl);

	/* Step2: Put [header] in sector 2 after [bootloader] */
	fseek(fp_si, 0, SEEK_END);
	len_si = ftell(fp_si);
	fseek(fp_si, 0, SEEK_SET);
	for (i = 0; i < len_si; i++) {
		c = fgetc(fp_si);
		fputc(c, fp_bl);
	}

	/* Step3: Pad NOP after [header] */
	/* 2015/04/16 We are still working on header, ignore 32-bit kernel. */
	image_size *= 1024;// in bytes count.
	image_size -= (len_si + len_bl + 2);
	for (i = 0; i < image_size; i++)
		fputc(NOP, fp_bl);
	
	/* FIXME */
	sectors_header = len_si / SECTOR_SIZE;
	sectors_header++;
	lower_bound_of_sp = 0x90000 + sectors_header * SECTOR_SIZE;
	printf("\nCaveat: header's size is %d, need %d sectors.\n", len_si, sectors_header);
	printf("Caveat: The lower bound of %%sp is %X\n\n", lower_bound_of_sp);

	fclose(fp_bl);
	fclose(fp_si);
	return 0;
}
