/*
 * 2015/04/15 Created by Shan Yizhou.
 */
 
#ifndef BOOT_STRING_H
#define BOOT_STRING_H

typedef unsigned int	size_t;

void *memcpy(void *dst, const void *src, size_t len);
void *memset(void *dst, int c, size_t len);
int memcmp(const void *s1, const void *s2, size_t len);
int strcmp(const char *str1, const char *str2);
int strncmp(const char *str1, const char *str2, size_t len);
size_t strlen(const char *s);
size_t strnlen(const char *s, size_t maxlen);

#endif /* BOOT_STRING_H */
