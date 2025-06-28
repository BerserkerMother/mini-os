#ifndef FORMAT_H
#define FORMAT_H

#include<stdarg.h>
#include<kernel/file.h>

int vfprintf(file*, const char*, va_list);

#endif
