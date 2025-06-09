#include "vga.h"
#include "common.h"
#include <stddef.h>
#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_ADDR 0xb8000

static uint8_t terminal_color = VGA_COLOR_WHITE | (VGA_COLOR_BLACK << 4);
static volatile uint16_t *terminal_buffer = (uint16_t *)VGA_ADDR;
static uint8_t terminal_col = 0;

static inline uint8_t get_color(enum vga_color fg, enum vga_color bg) {
  return fg | (bg << 4);
}

void init_terminal(enum vga_color fg, enum vga_color bg) {
  // set vga color global
  terminal_color = get_color(fg, bg);
  for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
    terminal_buffer[i] = ' ';
  }
}

static uint16_t char_with_color(unsigned char c, uint8_t color) {
  return (uint16_t)c | ((uint16_t)color << 8);
}

static inline size_t get_index(size_t row, size_t col) {
  return row * VGA_WIDTH + col;
}

static void next_line() {
  for (size_t r = 1; r < VGA_HEIGHT; r++) {
    for (size_t c = 0; c < VGA_WIDTH; c++) {
      terminal_buffer[get_index(r - 1, c)] = terminal_buffer[get_index(r, c)];
    }
  }
  terminal_col = 0;
  for (size_t c = 0; c < VGA_WIDTH; c++) {
    terminal_buffer[get_index(VGA_HEIGHT - 1, c)] = ' ';
  }
}

static void print_single_colored(char c, uint8_t color) {
  size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + terminal_col;
  terminal_buffer[index] = char_with_color(c, color);

  terminal_col++;
  if (terminal_col == VGA_WIDTH) {
    terminal_col = 0;
    next_line();
  }
}

void _print_colored(const char *text, uint8_t color) {
  size_t len = strlen(text);
  for (size_t i = 0; i < len; i++) {
    char c = text[i];
    switch (c) {
    case '\n':
      next_line();
      break;
    default:
      print_single_colored(c, color);
      break;
    }
  }
}

void print_colored(const char *text, enum vga_color fg, enum vga_color bg) {
  uint8_t color = get_color(fg, bg);
  _print_colored(text, color);
}

void print(const char *text) { _print_colored(text, terminal_color); }
