#include <kernel/file.h>
#include <kernel/format.h>
#include <kernel/io.h>
#include <kernel/serial.h>
#include <stdarg.h>
#include <stdio.h>

#define BUFFER_LENGTH 128
#define PORT 0x3f8

int write_serial(const char* c) {
  int written = 0;
  for (; *c != '\0'; c++, written++) {
    outb(PORT, *c);
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
  file* f = serial_buffer();
  va_list parameters;
  va_start(parameters, format);
  int written = vfprintf(f, format, parameters);
  va_end(parameters);
  return written;
}
