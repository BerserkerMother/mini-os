#include <kernel/file.h>
#include <kernel/format.h>
#include <kernel/pic.h>
#include <kernel/serial.h>
#include <kernel/tty.h>
#include <kernel/vga.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "gdt.h"
#include "idt.h"

extern uint32_t page_directory;
extern uint32_t page_table_0;
extern uint32_t gdt_start;
extern uint32_t gdt_end;

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

void _enable_gdt(void) {
  asm("cli");
  setup_gdt_entry(&gdt_start);
  set_gdt((&gdt_start - &gdt_end - 1), &gdt_start);
  reload_segments();
  serialprint("GDT is set!\n");
}

void _enable_idt(void) {
  setup_idt();
  load_idt();
  serialprint("IDT is set!\n");
}

void _init_pic(void) {
  pic_initialize(0x20, 0x28);
  serialprint("PIC intialized\n");
}

void kernel_main(void) {
  terminal_initialize(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
  printf("Kernel Initialized!\n");
  _enable_gdt();
  _enable_idt();
  _init_pic();
  serialprint("CR0=0x%x\n", _read_cr0());
  asm("sti");
  for (;;) {
    asm("hlt");
  }
}
