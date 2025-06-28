#include <kernel/file.h>
#include <kernel/format.h>
#include <kernel/serial.h>
#include <stdarg.h>
#include <stdio.h>

#define BUFFER_LENGTH 128
#define PORT 0x3f8

static inline void outb(unsigned short port, unsigned char c) {
  __asm__ volatile("outb %0, %1" : : "a"(c), "Nd"(port) :);
}

int write_serial(const char* c) {
  int written = 0;
  for (; *c != '\0'; c++) {
    outb(PORT, *c);
    written++;
  }

  return written;
}

static file* serial_buffer() {
  static char buffer[BUFFER_LENGTH] = {0};
  static file file = {
      .buffer = buffer,
      .buff_len = BUFFER_LENGTH,
      .buff_i = 0,
      .ptrfnwrt = write_serial,
  };
  return &file;
}

int serialprint(const char* restrict format, ...) {
  file* file = serial_buffer();
  va_list parameters;
  va_start(parameters, format);
  int written = vfprintf(file, format, parameters);
  printf("written: %d", written);
  va_end(parameters);
  return written;
}
