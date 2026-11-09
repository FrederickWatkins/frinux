#include <kernel/string.h>
#include <stdint.h>

void *memcpy(void *dst, const void *src, size_t n)
{
    for(size_t i = 0; i < n; i++) {
        *(uint8_t*)dst = *(uint8_t*)src;
    }
    return dst;
}

void *memmove(void *dst, const void *src, size_t n)
{
    if (dst > src && dst < src + n) {
        for (size_t i = n; i > 0; i--) {
            ((uint8_t*)dst)[i - 1] = ((uint8_t*)src)[i - 1];
        }
    } else {
        for (size_t i = 0; i < n; i++) {
            ((uint8_t*)dst)[i] = ((uint8_t*)src)[i];
        }
    }

    return dst;
}

void *memset(void *dst, int c, size_t n)
{
    for(size_t i = 0; i < n; i++) {
        ((uint8_t*)dst)[i] = (uint8_t)c;
    }
    return dst;
}

int memcmp(const void *a, const void *b, size_t n)
{
    for(size_t i = 0; i < n; i++) {
        uint8_t a_val = ((uint8_t*)a)[i];
        uint8_t b_val = ((uint8_t*)b)[i];
        if(a_val < b_val) return -1;
        if(a_val > b_val) return 1;
    }
    return 0;
}

void *memchr(const void *s, int c, size_t n)
{
    for(size_t i = 0; i < n; i++) {
        uint8_t s_val = ((uint8_t*)s)[i];
        uint8_t c_val = (uint8_t)c;
        if(s_val == c_val) return (uint8_t*)s + i;
    }
    return NULL;
}

size_t strlen(const char *s)
{
    size_t i;
    for(i = 0; s[i] != '\0'; i++);
    return i;
}

size_t strnlen(const char *s, size_t max)
{
    size_t i;
    for(i = 0; s[i] != '\0' && i < max; i++);
    return i;
}

char *strcpy(char *dst, const char *src)
{
    char *start = dst;
    do {
        *dst = *src;
        src++;
        dst++;
    } while(*src != '\0');
    return start;
}

char *strncpy(char *dst, const char *src, size_t max)
{
    for(size_t i = 0; i < max; i++) {
        dst[i] = src[i];
        if(src[i] == '\0') {
            return dst;
        }
    }
    return dst;
}

char *strchr(const char *s, int c)
{
    for(; *s != '\0'; s++) {
        if(*s == c) return (char*)s;
    }
    return NULL;
}

char *strrchr(const char *s, int c)
{
    char* end = strchr(s, '\0');
    for(; end > s; end--) {
        if(*end == c) return end;
    }
    return NULL;
}

int strcmp(const char *a, const char *b)
{
    while(*a != '\0' && *b != '\0') {
        uint8_t a_val = *(uint8_t*)a;
        uint8_t b_val = *(uint8_t*)b;
        if(a_val < b_val) return -1;
        if(a_val > b_val) return 1;
        a++;
        b++;
    }
    return 0;
}

int strncmp(const char *a, const char *b, size_t n)
{
    while(*a != '\0' && *b != '\0' && n > 0) {
        uint8_t a_val = *(uint8_t*)a;
        uint8_t b_val = *(uint8_t*)b;
        if(a_val < b_val) return -1;
        if(a_val > b_val) return 1;
        a++;
        b++;
        n--;
    }
    return 0;
}
