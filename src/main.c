#include "vga.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

void kernel_main() {
  init_terminal(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
  char text[] = "In love with Wiiiish Taaaaraaaan.\n";
  for (int i = 0; i < 16; i++) {
    print(text);
    print_colored(text, VGA_COLOR_YELLOW, VGA_COLOR_LIGHT_BLUE);
  }
}
