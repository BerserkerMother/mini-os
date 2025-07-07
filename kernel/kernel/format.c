#include <kernel/file.h>
#include <kernel/format.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 64
static char write_buffer[BUFFER_SIZE] = {0};

static int flush_file(file* file) {
  file->buffer[file->buff_len - 1] = '\0';
  if ((size_t)file->ptrfnwrt(file->buffer) != file->buff_i) {
    return -1;
  }

  memset(file->buffer, 0, file->buff_len);
  file->buff_i = 0;
  return 0;
}

static int push_to_buf(file* file, char c) {
  file->buffer[file->buff_i] = c;
  file->buff_i++;

  int err = 0;
  if (file->buff_i == file->buff_len - 1) {
    err = flush_file(file);
  }
  return err;
}

static int print(file* file, const char* s, size_t length) {
  for (size_t i = 0; i < length; i++) {
    if (s[i] == '\0') return -1;
    int err = push_to_buf(file, s[i]);
    if (err != 0) {
      return err;
    }
  }
  return (int)length;
}

static int printnum(file* file, uint32_t num, size_t base) {
  int i = BUFFER_SIZE - 2;
  for (; num; num /= base, i--) {
    write_buffer[i] = "0123456789abcdefghijklmnopqrstuvwxyz"[num % base];
  }
  size_t length = BUFFER_SIZE - 2 - i;
  int err = print(file, &write_buffer[i + 1], length);
  if (err == -1) {
    return -1;
  }

  return (int)length;
}

int vfprintf(file* file, const char* format, va_list parameters) {
  int written = 0;
  while (*format != '\0') {
    if (*format != '%' || format[1] == '%') {
      if (*format == '%') {
        format++;
      }
      if (push_to_buf(file, *format) == -1) return -1;
      written++;
      format++;
      continue;
    }

    // skip %
    format++;

    switch (*format) {
      case 'c': {
        char c = (char)va_arg(parameters, int);
        if (print(file, &c, sizeof(char)) == -1) return -1;
        format++;
        written++;
      } break;
      case 's': {
        const char* s = va_arg(parameters, const char*);
        int length = strlen(s);
        if (print(file, s, length) == -1) return -1;
        format++;
        written += length;
      } break;
      case 'd': {
        int num = va_arg(parameters, int);
        int length = printnum(file, num, 10);
        if (length == -1) {
          return -1;
        }
        format++;
        written += length;
      } break;
      case 'x': {
        uint32_t num = (uint32_t)va_arg(parameters, int);
        int length = printnum(file, num, 16);
        if (length == -1) {
          return -1;
        }
        format++;
        written += length;
      } break;
      default: {
        continue;
      }
    }
  }
  int err = flush_file(file);
  if (err != 0) {
    return err;
  }
  return written;
}
