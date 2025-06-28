#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MAX_INT_LENGTH 60
static unsigned char number_p[MAX_INT_LENGTH];

static bool printnum_with_base(uint32_t num, int base) {
  size_t i = MAX_INT_LENGTH - 1;

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
    if (putchar(number_p[i]) == EOF) return false;
  }

  return true;
}

static bool print(const char *data, size_t length) {
  const unsigned char *bytes = (const unsigned char *)data;
  for (size_t i = 0; i < length; i++)
    if (putchar(bytes[i]) == EOF) return false;
  return true;
}

int printf(const char *restrict format, ...) {
  va_list parameters;
  va_start(parameters, format);

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
      if (!print(format, amount)) return -1;
      format += amount;
      written += amount;
      continue;
    }

    const char *format_begun_at = format++;

    switch (*format) {
      case 'c': {
        format++;
        char c = (char)va_arg(parameters, int /* char promotes to int */);
        if (!maxrem) {
          // TODO: Set errno to EOVERFLOW.
          return -1;
        }
        if (!print(&c, sizeof(c))) return -1;
        written++;
      } break;
      case 's': {
        format++;
        const char *str = va_arg(parameters, const char *);
        size_t len = strlen(str);
        if (maxrem < len) {
          // TODO: Set errno to EOVERFLOW.
          return -1;
        }
        if (!print(str, len)) return -1;
        written += len;
      } break;
      case 'd': {
        int num = va_arg(parameters, int);
        if (!printnum_with_base(num, 10)) return -1;
        written += 1;
        format++;
      } break;
      case 'x': {
        if (!print("0x", 2)) return -1;
        int num = va_arg(parameters, int);
        if (!printnum_with_base(num, 16)) return -1;
        written += 1;
        format++;
      } break;
      default: {
        format = format_begun_at;
        size_t len = strlen(format);
        if (maxrem < len) {
          // TODO: Set errno to EOVERFLOW.
          return -1;
        }
        if (!print(format, len)) return -1;
        written += len;
        format += len;
      }
    }
  }
  va_end(parameters);
  return written;
}
