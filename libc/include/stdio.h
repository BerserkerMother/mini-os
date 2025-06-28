#ifndef _STDIO_H
#define _STDIO_H 1

#include <sys/cdefs.h>
#include <stdarg.h>

#define EOF (-1)

#ifdef __cplusplus
extern "C" {
#endif

int printf(const char* __restrict, ...);
int sprintf(unsigned char*, const char* __restrict, ...);
int svprintf(unsigned char*, va_list, const char* format);
int putchar(int);
int puts(const char*);

#ifdef __cplusplus
}
#endif

#endif
