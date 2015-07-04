#include <sandix/types.h>
#include <sandix/string.h>

/*	
 *	The original code incline to use local variables as
 *	the output operants in inline assembly to avoid wrting
 *	the clobber list, and, i guess, maybe easy for debug.
 *	It is a little tricky, but making the code clean.
 */

#ifdef __X86_SPECIFIC_STRING

char *strcpy(char *dest, const char *src)
{
	int d0, d1, d2;
	asm volatile (
		"1:\tlodsb\n\t"
		"stosb\n\t"
		"testb %%al, %%al\n\t"
		"jne 1b"
		: "=&S"(d0), "=&D"(d1), "=a"(d2)
		: "0"(src), "1"(dest)
		: "memory"
	);
	return dest;
}

int strcmp(const char *ss, const char *st)
{
	int ret, d0, d1;
	asm volatile (
		"1:\tlodsb\n\t"
		"scasb\n\t"
		"jne 2f\n\t"
		"testb %%al, %%al\n\t"
		"jne 1b\n\t"
		"xorl %%eax, %%eax\n\t"
		"jmp 3f\n\t"
		"2:\tsbbl %%eax, %%eax\n\t"
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
	int d0;
	size_t len;
	asm volatile (
		"repne; scasb"
		: "=&D" (d0), "=c" (len)
		: "0" (s), "1" (0xffffffffu), "a" (0)
		: "memory"
	);
	return ~len - 1;
}

void *memset(void *buf, int val, size_t len)
{
	int d0, d1, d2;
	asm volatile (
		"rep\n\t"
		"stosb\n\t"
		: "=&D" (d0), "=&D" (d1), "=&c" (d1)
		: "0" (buf), "1" (val), "2" (len)
		: "memory"
	);
}

#endif /* __X86_SPECIFIC_STRING */
