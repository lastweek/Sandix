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

#define NOP			90
#define SECTOR_SIZE 512
const char	MAGIC_510	= 0x55;
const char	MAGIC_511	= 0xAA;
const char *bootloader	= "bootsect";

// just need output image size.
int main(int argc, char **argv) {
	FILE *fp_bl, *fp_si, *fp_ki;
	int len_bl, len_si, len_ki;
	int image_size;	// 1M, 2M...
	char pad[SECTOR_SIZE] = {'\0'};

	if (argc > 2) {
		printf("Too many arguments\n");
		exit(1);
	}

	fp_bl = fopen(bootloader,"r+");
	if (fp_bl == NULL) {
		printf("Open bootsect failed\n");
		exit(1);
	}
	
	// Step1: Padding the hole in bootloader.
	fseek(fp_bl, -1, SEEK_END);
	len_bl = ftell(fp_bl);	// Skip EOF in the end.
	printf("bootsect length: %d bytes\n", len_bl);
	
	//Padding the hole with NOP
	while (len_bl < (SECTOR_SIZE - 2)) {
		fputc(NOP, fp_bl);
		len_bl++;
	}
	//Now the magic.
	fputc(MAGIC_510, fp_bl);
	fputc(MAGIC_511, fp_bl);

	// Step2: Enlarge 
	image_size = atoi(argv[argc-1]);
	if (!image_size) {
		printf("Invalid size\n");
		exit(1);
	}
	image_size = image_size*1024 - 512;
	for (int i = 0; i < image_size; i++) {
		fputc('\0', fp_bl);
	}

	fclose(fp_bl);
	return 0;
}
