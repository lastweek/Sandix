/* String Manipulations */

#define __X86_SPECIFIC_STRING

extern char *strcpy(char *dest, const char *src);
extern char *strcmp(const char *s1, const char *s2);
extern size_t strlen(const char *s);

extern void *memset(void *buf, int val, size_t len);
