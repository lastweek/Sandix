#include <sandix/types.h>
#include <sandix/string.h>

/*	
 *	The original code incline to use local variables as
 *	the output operants in inline assembly to avoid wrting
 *	the clobber list. It is a little tricky, but making the
 *	code clean and concise.
 */

#ifdef __X86_SPECIFIC_STRING

char *strcpy(char *dest, const char *src)
{
	int d0, d1, d2;
	asm volatile (
		"1:\n\t"
		"lodsb\n\t"
		"stosb\n\t"
		"testb %%al, %%al\n\t"
		"jne 1b"
		: "=&S"(d0), "=&D"(d1), "=&a"(d)
		: "0"(src), "1"(dest)
		: "memory"
	);
	return dest;
}

int strcmp(const char *ss, const char *st)
{
	int ret, d0, d1;
	asm volatile (
		"1:\n\t"
		"lodsb\n\t"
		"scasb\n\t"
		"jne 2f\n\t"
		"testb %%al, %%al\n\t"
		"jne 1b\n\t"
		"xorl %%eax, %%eax\n\t"
		"jmp 3f\n\t"
		"2:\n\t"
		"sbbl %%eax, %%eax\n\t"
		"orb $1, %%al\n\t"
		"3:"
		: "=&S"(d0), "=&D"(d1), "=a"(ret)
		: "0"(ss), "1"(st)
		: "memory"
	);
	return ret;
}

size_t strlen(const char *s)
{
	size_t len = 0;
	asm volatile (
		
	);
	return len;
}

void *memset(void *buf, int val, size_t len)
{
	asm volatile (
		"rep\n\t"
		"stosb\n\t"
		: : "D" (buf), "a" (val), "c" (len)
	);
}

#endif /* __X86_SPECIFIC_STRING */
