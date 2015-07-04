/* String Manipulations */

#define __X86_SPECIFIC_STRING

char *strcpy(char *dest, const char *src);
char *strcmp(const char *s1, const char *s2);
size_t strlen(const char *s);

void *memset(void *buf, int val, size_t len);
