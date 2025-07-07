#ifndef GDT_H
#define GDT_H

#include <stdint.h>

#define GDT_BASE 0x300000

struct GDT {
  uint32_t base;
  uint32_t limit;
  uint8_t flags;
  uint8_t access_byte;
};

extern void set_gdt(uint16_t limit, uint32_t base);
extern void reload_segments();
void setup_gdt_entry(uint32_t target);
void encode_gdt_entry(uint8_t* target, struct GDT source);

#endif
