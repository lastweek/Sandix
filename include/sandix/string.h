#ifndef _SANDIX_STRING_H_
#define _SANDIX_STRING_H_

#include <sandix/types.h>

/* arch specific string functions */
#define __X86_SPECIFIC_STRING

char *strcpy(char *dest, const char *src);
int strcmp(const char *s0, const char *s1);
size_t strlen(const char *s);

void *memset(void *s, char c, size_t n);
void *memcpy(void *to, const void *from, size_t n);

#endif /* _SANDIX_STRING_H_ */
