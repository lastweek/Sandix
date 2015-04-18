#include <stdio.h>

int main()
{
	int x = 100;
	int *p = &x;
	char *s;
	char *t = "asd";
	char **q = &t;

	printf("%x \n%X \n%o\n", p, p, p, p);
	
	s = *(char * *)q;
	printf("%s\n",s);
	return 0;
}
