#include <stdio.h>

int __strcmp(const char *cs, const char *ct)
{
	int d0, d1;
	int res;
	asm volatile("1:\tlodsb\n\t"
		"scasb\n\t"
		"jne 2f\n\t"
		"testb %%al,%%al\n\t"
		"jne 1b\n\t"
		"xorl %%eax,%%eax\n\t"
		"jmp 3f\n"
		"2:\tsbbl %%eax,%%eax\n\t"
		"orb $1,%%al\n"
		"3:"
		: "=a" (res), "=&S" (d0), "=&D" (d1)
		: "1" (cs), "2" (ct)
		: "memory");
	return res;
}

void *__memset(void *s, char c, size_t n)
{
	int d0, d1, d2;
	asm volatile (
		"rep\n\t"
		"stosb\n\t"
		: "=&D" (d0), "=a" (d1), "=&c" (d2)
		: "0" (s), "1" (c), "2" (n)
		: "memory"
	);
}

int m_strcmp(const char *ss, const char *st)
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
int main()
{
	char a[3] = "123";
	char b[3] = "123";
	int x;
	
	x = m_strcmp(b, a);

	printf("%d\n", x);

	return 0;
}
