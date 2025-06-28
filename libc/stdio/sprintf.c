#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MAX_INT_LENGTH 60
static unsigned char number_p[MAX_INT_LENGTH];

static bool print(char* buffer, const char* data, size_t length) {
  memcpy(buffer, data, length);
  return true;
}

static int printnum_with_base(char* buffer, uint32_t num, int base) {
  size_t i = MAX_INT_LENGTH - 1;
  int written = 0;

  if (!num) {  // handle 0
    number_p[i] = '0';
    i--;
  }

  while (num) {
    int reminder = num % base;
    number_p[i] = "0123456789abcdefghijklmnopqrstuvwxyz"[reminder];
    num /= base;
    i--;
  }
  i++;  // last decerement is unneccessary!

  for (; i < MAX_INT_LENGTH; i++) {
    if (print(buffer, number_p[i], 1) == EOF) return false;
    buffer++;
    written++;
  }

  return written;
}

int sprintf(unsigned char* buffer, const char* restrict format, ...) {
  va_list parameters;
  va_start(parameters, format);

  int written = svprintf(buffer, parameters, format);

  va_end(parameters);
  return written;
}

int svprintf(unsigned char* buffer, va_list parameters, const char* format) {
  int written = 0;

  while (*format != '\0') {
    size_t maxrem = INT_MAX - written;

    if (format[0] != '%' || format[1] == '%') {
      if (format[0] == '%') format++;
      size_t amount = 1;
      while (format[amount] && format[amount] != '%') amount++;
      if (maxrem < amount) {
        // TODO: Set errno to EOVERFLOW.
        return -1;
      }
      if (!print(buffer, format, amount)) return -1;
      buffer += amount;
      format += amount;
      written += amount;
      continue;
    }

    const char* format_begun_at = format++;

    switch (*format) {
      case 'c': {
        format++;
        char c = (char)va_arg(parameters, int /* char promotes to int */);
        if (!maxrem) {
          // TODO: Set errno to EOVERFLOW.
          return -1;
        }
        if (!print(buffer, &c, sizeof(c))) return -1;
        written++;
        buffer++;
      } break;
      case 's': {
        format++;
        const char* str = va_arg(parameters, const char*);
        size_t len = strlen(str);
        if (maxrem < len) {
          // TODO: Set errno to EOVERFLOW.
          return -1;
        }
        if (!print(buffer, str, len)) return -1;
        written += len;
        buffer += len;
      } break;
      case 'd': {
        int num = va_arg(parameters, int);
        int len = printnum_with_base(buffer, num, 10);
        if (len == EOF) return -1;
        written += len;
        buffer += len;
        format++;
      } break;
      case 'x': {
        if (!print(buffer, "0x", 2)) return -1;
        int num = va_arg(parameters, int);
        int len = printnum_with_base(buffer, num, 16);
        if (len == EOF) return -1;
        written += len;
        buffer += len;
        format++;
      } break;
      default: {
        format = format_begun_at;
        size_t len = strlen(format);
        if (maxrem < len) {
          // TODO: Set errno to EOVERFLOW.
          return -1;
        }
        if (!print(buffer, format, len)) return -1;
        written += len;
        format += len;
        buffer += len;
      }
    }
  }
  return written;
}
