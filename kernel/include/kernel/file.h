#ifndef FILE_H
#define FILE_H

#include <stddef.h>

typedef int (*ptrfnwrt) (const char*);
typedef struct {
  size_t buff_len;
  size_t buff_i;
  char* buffer;
  ptrfnwrt ptrfnwrt;
} file;

#endif
