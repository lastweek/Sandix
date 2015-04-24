#include <stdio.h>

int get_mode()
{
	unsigned char c = 0x88;
	int t = c;
	return c;
}

int main()
{
	char c;
	int sz;
	unsigned int uz;

	uz = 0;
	printf("unsigned:\t%x %u\n", uz-1, uz-1);

	sz = 0;
	printf("signed  :\t%x %d\n", sz-1, sz-1);

	c = 0xff;
	uz = (unsigned char)c;
	//uz = (unsigned int)(c-'\0');
	sz = c;
	printf("%x %x\n", uz, sz);

	sz = get_mode();
	printf("%X\n", sz);
}
