#include <kernel/file.h>
#include <kernel/format.h>
#include <kernel/serial.h>
#include <kernel/tty.h>
#include <kernel/vga.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

extern uint32_t page_directory;
extern uint32_t page_table_0;

static inline uint32_t _read_cr0(void) {
  uint32_t cr0;
  __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
  return cr0;
}

static inline uint32_t _read_cr3(void) {
  uint32_t cr3;
  __asm__ volatile("mov %%cr3, %0" : "=r"(cr3));
  return cr3;
}

bool _is_paging_enabled(void) {
  uint32_t cr0 = _read_cr0();
  return (cr0 & 0x80000000) != 0;
}

void kernel_main(void) {
  terminal_initialize(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
  printf("Kernel Initialized!\n");
  serialprint("Hello World!\n%x", 128);
}
