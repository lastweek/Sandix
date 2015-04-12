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
	FILE *fp_bl, *fp_si, *fp_ki;	// Input three files
	FILE *ofp;						// Output bzimage
	int len_bl, len_si, len_ki;		// Input files's length
	int image_size;					// 1M, 2M...
	int i;

	if (argc > 2) {
		err("Too many arguments\n");

	if ((image_size = atoi(argv[argc-1])) == 0)
		err("Please specify a valid final image size.\n");
	image_size = image_size*1024 - 512;// in bytes count.

	if ((fp_bl = fopen(bootloader, "r+")) == NULL)
		err("Open [bootsect] failed\n");

	if ((fp_si = fopen(setup, "r+")) == NULL)
		err("Open [header] failed\n");

	/* Step1: Padding the hole in bootloader. */
	fseek(fp_bl, -1, SEEK_END);// Skip EOF in the end.
	if ((len_bl = ftell(fp_bl)) > (SECTOR_SIZE - 2))
		err("[bootsect] is too large to form MBR\n")
	
	//Padding the hole with NOP
	while (len_bl < (SECTOR_SIZE - 2)) {
		fputc(NOP, fp_bl);
		len_bl++;
	}
	//Now the magic.
	fputc(MAGIC_510, fp_bl);
	fputc(MAGIC_511, fp_bl);

	/* Step2: Put [header] in sector 2 after [bootloader] */
	fseek(fp_si, -1, SEEK_END);
	len_si = ftell(fp_si);
	
	
	fclose(fp_bl);
	fclose(fp_si);
	return 0;
}

