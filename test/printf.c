#include <stdio.h>

int main()
{
	unsigned int unum;
	int num = 0xFFFFFFFF;
	int x = 100;
	int *p = &x;
	char *s;
	char *t = "asd";
	char **q = &t;

	printf("%u \n%X \n%o\n", num, p, p, p);
	
	s = *(char * *)q;	// This is ok!
	printf("%s\n",s);

	unum = (unsigned int)num;
	printf("%X\n", unum);
	
	x = num/10;
	printf("%d %u\n", x, x);
	
	return 0;
}
