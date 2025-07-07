#ifndef IDT_H
#define IDT_H

#include <stdint.h>

// extern uint32 idt_start;
// extern uint32 idt_end;

struct InterruptDescriptor{
  uint32_t offset;
  uint16_t segment_selector;
  uint8_t gate_type;
  uint8_t dpl;
  uint8_t present;
};

void encode_idt_entry(uint8_t* target, struct InterruptDescriptor id);
void setup_idt(void);
void load_idt(void);

#endif
