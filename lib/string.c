#include <sandix/types.h>

char *strcpy(char *dest, const char *src)
{
	asm volatile (
		"1:\n\t"
		"lodsb\n\t"
		"stosb\n\t"
		"testb %%al, %%al\n\t"
		"jne 1b"
		: : "S"(src), "D"(dest)
	);
	return dest;
}

int strcmp(const char *s1, const char *s2)
{
	int ret;
	asm volatile (
		
	);
}

size_t strlen(const char *s)
{

}

void *memset(void *buf, int val, size_t len)
{
	
}
