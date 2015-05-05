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
	int sectors_header;				// Sectors of header
	int sectors_image;				// Sectors of image
	int sectors_bzimage;			// Sectors of bzimage
	int lower_bound_of_sp;			// The lower bound of stack point, %sp
	int i, pad;
	char c;

	if ((fp_bl = fopen(bootloader, "r+")) == NULL)
		err("Open [bootsect] failed\n");

	if ((fp_si = fopen(setup, "r+")) == NULL)
		err("Open [header] failed\n");
	
	if ((fp_ki = fopen(kernel, "r+")) == NULL)
		err("Open [image] failed\n");

	
	/*
	 * Step1: Put [bootloader] in MBR.
	 */
	fseek(fp_bl, -1, SEEK_END);// Skip EOF in the end.
	if ((len_bl = ftell(fp_bl)) > (SECTOR_SIZE - 2))
		err("[bootsect] is too large to form MBR\n");
	
	// Padding the hole with NOP and magic to 512 bytes.
	while (len_bl < (SECTOR_SIZE - 2)) {
		fputc(NOP, fp_bl);
		len_bl++;
	}
	fputc(MAGIC_510, fp_bl);
	fputc(MAGIC_511, fp_bl);

	/*
	 * Step2: Catenate [header] after [bootloader].
	 */
	fseek(fp_si, 0, SEEK_END);
	len_si = ftell(fp_si);
	fseek(fp_si, 0, SEEK_SET);
	for (i = 0; i < len_si; i++) {
		c = fgetc(fp_si);
		fputc(c, fp_bl);
	}
	
	// Padding the hole with NOP, make [header] 512 byte alignment.
	if (pad = len_si % SECTOR_SIZE) {
		pad = SECTOR_SIZE - pad;
		for (i = 0; i < pad; i++)
			fputc(NOP, fp_bl);
	}

	/*
	 * Step3: Catenate [image] after [header].
	 */
	 fseek(fp_ki, 0, SEEK_END);
	 len_ki = ftell(fp_ki);
	 fseek(fp_ki, 0, SEEK_SET);
	 for (i = 0; i < len_ki; i++) {
	 	c = fgetc(fp_ki);
		fputc(c, fp_bl);
	 }
	
	// Padding the hole with NOP, make [image] 512 byte alignment.
	if (pad = len_ki % SECTOR_SIZE) {
		pad = SECTOR_SIZE - pad;
		for (i = 0; i < pad; i++)
			fputc(NOP, fp_bl);
	}

	/*
	 * Step4: Print some infomation.
	 */
	sectors_header    = (len_si % SECTOR_SIZE)? (len_si/SECTOR_SIZE+1): len_si/SECTOR_SIZE;
	sectors_image     = (len_ki % SECTOR_SIZE)? (len_ki/SECTOR_SIZE+1): len_ki/SECTOR_SIZE;
	sectors_bzimage   = sectors_header + sectors_image + 1;
	lower_bound_of_sp = sectors_header * SECTOR_SIZE + 0x90000;

	printf("\nCaveat: [header]'s size is %d bytes, need %d sectors.\n", len_si, sectors_header);
	printf("Caveat: [image]'s size is %d bytes, need %d sectors.\n", len_ki, sectors_image);
	printf("Caveat: [bzimage]'s size is %d bytes, total %d sectors.\n", sectors_bzimage*SECTOR_SIZE, sectors_bzimage);
	printf("Caveat: The lower bound of %%sp is %X\n\n", lower_bound_of_sp);

	fclose(fp_bl);
	fclose(fp_si);
	fclose(fp_ki);

	return 0;
}
