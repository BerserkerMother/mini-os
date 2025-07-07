#include "gdt.h"

#include <kernel/serial.h>
#include <stdint.h>
#include <stdio.h>

void encode_gdt_entry(uint8_t* target, struct GDT source) {
  if (source.limit > 0xFFFFF)
    printf("[ERR] GDT cannot encode, limit larger than 0xFFFFF");

  // Encode the limit
  target[0] = source.limit & 0xFF;
  target[1] = (source.limit >> 8) & 0xFF;
  target[6] = (source.limit >> 16) & 0x0F;

  // Encode the base
  target[2] = source.base & 0xFF;
  target[3] = (source.base >> 8) & 0xFF;
  target[4] = (source.base >> 16) & 0xFF;
  target[7] = (source.base >> 24) & 0xFF;

  // Encode the access byte
  target[5] = source.access_byte;

  // Encode the flags
  target[6] |= (source.flags << 4);
}

void setup_gdt_entry(uint32_t target) {
  uint8_t* base = (uint8_t*)target;
  struct GDT null_gdt = {.base = 0, .limit = 0, .access_byte = 0, .flags = 0};
  encode_gdt_entry(base, null_gdt);

  struct GDT kernel_code_gdt = {
      .base = 0x0, .limit = 0xFFFFF, .access_byte = 0x9A, .flags = 0xC};
  encode_gdt_entry(base + 8, kernel_code_gdt);

  struct GDT kernel_data_gdt = {
      .base = 0x0, .limit = 0xFFFFF, .access_byte = 0x92, .flags = 0xC};
  encode_gdt_entry(base + 16, kernel_data_gdt);
}
