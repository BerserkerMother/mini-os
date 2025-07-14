#include <kernel/io.h>

void outb(unsigned short port, unsigned char c) {
  __asm__ volatile("outb %0, %1" : : "a"(c), "Nd"(port) :);
}

unsigned char inb(unsigned short port) {
  unsigned char ret;
  __asm__ volatile("inb %w1, %b0" : "=a"(ret) : "Nd"(port) : "memory");
  return ret;
}

void io_wait() { outb(0x80, 0); }
