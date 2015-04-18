#include <stdio.h>

int main()
{
	int x = 100;
	int *p = &x;

	printf("%x \n%X \n%o \n%O", p, p, p, p);

	return 0;
}
