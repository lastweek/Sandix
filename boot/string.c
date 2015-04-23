/*
 *	2015/04/16 Created by Shan Yizhou
 *
 *	string.c: Simple string manipulate functions.
 */

typedef unsigned int	size_t;

// The memset() function writes len bytes of
// value c (converted to an unsigned char) to the string b.
// The memset() function returns its first argument.
void *memset(void *dst, int c, size_t len)
{
	char *d = (char *)dst;
	
	while (len--)
		*d++ = (char)c;
	return dst;
}

// The memcpy() function copies n bytes from memory area src to memory area dst.
// If dst and src overlap, behavior is undefined.
// The memcpy() function returns the original value of dst.
void *memcpy(void *dst, const void *src, size_t len)
{
	char *s = (char *)src;
	char *d = (char *)dst;

	while(len && *s) {
		*d++ = *s++;
		len--;
	}
	return dst;
}

// I donot know what's going on...
// Derived from linux kernel.
int memcmp(const void *s1, const void *s2, size_t len)
{
	unsigned char diff;
	asm("repe; cmpsb; setnz %0"
	    : "=qm" (diff), "+D" (s1), "+S" (s2), "+c" (len));
	return diff;
}

// Compare the null-terminated strings s1 and s2.
// Because strcmp() is designed for comparing strings rather
// than binary data, characters that appear after a `\0' are not compared.
int strcmp(const char *str1, const char *str2)
{
	const unsigned char *s1 = (const unsigned char *)str1;
	const unsigned char *s2 = (const unsigned char *)str2;
	int delta = 0;

	while (*s1 || *s2) {
		delta = *s2 - *s1;
		if (delta)
			return delta;
		s1++;
		s2++;
	}
	return 0;
}
// The strncmp() function compares not more than len characters.
int strncmp(const char *str1, const char *str2, size_t len)
{
	const unsigned char *s1 = (const unsigned char *)str1;
	const unsigned char *s2 = (const unsigned char *)str2;
	int delta = 0;

	while ((*s1 || *s2) && len) {
		delta = *s2- *s1;
		if (delta)
			return delta;
		s1++;
		s2++;
		len--;
	}
	return 0;
}

//The strlen() function attempts to compute the length of s.
size_t strlen(const char *s)
{
	const char *es = s;
	
	for (es = s; *es != '\0'; es++)
		/* nothing */;
	return (es - s);
}

// The strnlen() function attempts to compute the length of s,
// but never scans beyond the first maxlen bytes of s.
size_t strnlen(const char *s, size_t maxlen)
{
	const char *es = s;
	
	while (*es && maxlen) {
		es++;
		maxlen--;
	}
	return (es - s);
}

