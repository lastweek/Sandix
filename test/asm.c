#include <stdio.h>

#define LOCK "lock; "

char *_strcpy(char *dest, const char *src)
{
	asm volatile(
		"1:\tlodsb\n\t"
		"stosb\n\t"
		"testb %%al,%%al\n\t"
		"jne 1b"
		:
		: "S" (src), "D" (dest)
		: "memory", "%eax");
	return dest;
}

char *strcpy(char *dest, const char *src)
{
	int d0, d1, d2;
	
	asm volatile("1:\tlodsb\n\t"
		"stosb\n\t"
		"testb %%al,%%al\n\t"
		"jne 1b"
		: "=&S" (d0), "=&D" (d1), "=&a" (d2)
		: "0" (src), "1" (dest) : "memory");
	return dest;
}

int main(){
	char a[5]="12345";
	char b[5];

	strcpy(b,a);
	_strcpy(b,a);
	
	return 0;
}
