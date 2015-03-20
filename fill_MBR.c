// Fill binary file to 512 bytes.
// The last two bytes are MAGIC NUMBER.
// 2015/03/22 Created by Shan Yizhou.

#include <stdio.h>
#include <stdlib.h>

const char	MAGIC_510	= 0x55;
const char	MAGIC_511	= 0xAA;
const int 	MBR_SIZE	= 512;	

int main(int argc, char **argv) {
	FILE *ifp;
	int size;
	char c;

	if (argc > 2) {
		printf("Too many arguments\n");
		exit(1);
	}

	ifp = fopen(argv[argc-1],"r+");
	if (ifp == NULL) {
		printf("Open %s failed\n", argv[argc-1]);
		exit(1);
	}
	
	fseek(ifp, -1, SEEK_END);
	size = ftell(ifp);	// Skip EOF in the end.
	printf("Input File Size: %d bytes\n", size);
	
	//Padding the hole.
	while (size < (MBR_SIZE - 2)) {
		fputc(0x00, ifp);
		size++;
	}
	//Now the magic.
	fputc(MAGIC_510, ifp);
	fputc(MAGIC_511, ifp);

	fclose(ifp);
	return 0;
}
