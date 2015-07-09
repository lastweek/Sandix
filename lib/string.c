#include <sandix/types.h>
#include <sandix/string.h>

#ifdef __X86_SPECIFIC_STRING /* in <sandix/string.h> */

/*
 * I have read the LKML about why using "memory"
 * clobber in all string functions including strlen().
 * As Linus said, even if you do NOT modify memory
 * in inline assembly, you should make sure GCC
 * had already flush the "dirty" data back to memory.
 * For example, when you are using strlen(), if GCC
 * has put some modified data in registers without
 * flushing back to memory before you call strlen(),
 * then a mistake happens. So it is NECESSARY to put
 * "memory" in the clobber list.
 *
 * And why all of them using local variables like d0,
 * d1 still confusing me. Maybe linux guys just dont
 * wanna writing extra registers in the clobber list
 * in order to make it clean?
 */

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
		"repne ; scasb"
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
