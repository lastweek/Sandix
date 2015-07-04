#include <sandix/types.h>
#include <sandix/string.h>

/*	
 *	The original code incline to use local variables as
 *	the output operants in inline assembly to avoid wrting
 *	the clobber list, and, i guess, maybe easy for debug.
 *	It is a little tricky, but making the code clean(?!).
 *	Compile with -O2 enabled, the output operants relevant
 *	local variables do not have any affect.
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
		: "=&S"(d0), "=&D"(d1), "=a"(d2)
		: "0"(src), "1"(dest)
		: "memory"
	);
	return dest;
}

int strcmp(const char *s0, const char *s1)
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
		: "0"(s0), "1"(s1)
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

void *memset(void *s, char c, size_t n)
{
	int d0, d1, d2;
	asm volatile (
		"rep ; stosb"
		: "=&D" (d0), "=a" (d1), "=&c" (d2)
		: "0" (s), "1" (c), "2" (n)
		: "memory"
	);
	return s;
}

void *memcpy(void *to, const void *from, size_t n)
{
	int d0, d1, d2;
	asm volatile (
		"rep ; movsl\n\t"
		"movl %3, %%ecx\n\t"
		"andl $3, %%ecx\n\t"
		"jz 1f\n\t"
		"rep ; movsb\n\t"
		"1:"
		: "=&D" (d0), "=&S" (d1), "=&c" (d2)
		: "g" (n), "0" (to), "1" (from), "2" (n / 4)
		: "memory"
	);
	return to;
}

#endif /* __X86_SPECIFIC_STRING */
