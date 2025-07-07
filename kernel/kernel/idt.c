#include "idt.h"

#include <kernel/serial.h>
#include <stdint.h>

static uint64_t IDT[256] = {0};

static int _read_cr2(void){
  int cr2 = 0;
  __asm__ volatile(
      "mov %%cr2, %0"
      :"=r"(cr2)
      );
  return cr2;
}

struct __attribute__((packed)) idtr {
  uint16_t size;
  uint32_t offset;
};

// A struct matching exactly the layout we're about to push in Assembly
typedef struct {
  uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
  uint32_t eip;
  uint32_t cs;
  uint32_t eflags;
} regs_t;

void encode_idt_entry(uint8_t* target, struct InterruptDescriptor id) {
  // offset first 16 bit
  target[0] = id.offset & 0xff;
  target[1] = (id.offset >> 8) & 0xff;

  // segement selector
  target[2] = id.segment_selector & 0xff;
  target[3] = (id.segment_selector >> 8) & 0xff;

  // reserved
  target[4] = 0x00;
  // target[5] = (0xE & 0x0F) | (0<<5) | (1<<7);
  // gatetype(4) 0 dpl(2) presenet(1)
  target[5] = (id.gate_type & 0x0F) | (id.dpl << 5) | (id.present << 7);

  target[6] = (id.offset >> 16) & 0xff;
  target[7] = (id.offset >> 24) & 0xff;
}

void load_idt(void) {
  struct idtr idtr = {.size = sizeof(IDT) - 1, .offset = &IDT};
  __asm__ volatile("lidt %0" : : "m"(idtr) : "memory");
}

void _isr8_handler(regs_t stack_frame, uint32_t err_code, uint32_t int_no) {
  serialprint("[INT] number %d\n", int_no);
  serialprint("DOUBLE FALUT!\n");
  asm("hlt");
}

void _isr13_handler(regs_t stack_frame, uint32_t err_code, uint32_t int_no) {
  serialprint("[INT] number %d\n", int_no);
  serialprint("GENERAL PROTECTION!\n");
  asm("hlt");
}

void _isr14_handler(regs_t stack_frame, uint32_t err_code, uint32_t int_no) {
  serialprint("[INT] number %d\n", int_no);
  int cr2 = _read_cr2();
  serialprint("PAGE FAULT! Error code: %d Accessed page: 0x%x\n", err_code, cr2);
  asm("hlt");
}

void setup_idt(void) {
  extern isr8_handler(void);
  struct InterruptDescriptor dfd = {.offset = (uint32_t)isr8_handler,
                                    .segment_selector = 0x08,
                                    .dpl = 0x00,
                                    .present = 0x01,
                                    .gate_type = 0x0e};
  extern isr14_handler(void);
  struct InterruptDescriptor pfd = {.offset = (uint32_t)isr14_handler,
                                    .segment_selector = 0x08,
                                    .dpl = 0x00,
                                    .present = 0x01,
                                    .gate_type = 0x0e};

  extern isr13_handler(void);
  struct InterruptDescriptor general_protection_fault = {
      .offset = (uint32_t)isr13_handler,
      .segment_selector = 0x08,
      .dpl = 0x00,
      .present = 0x01,
      .gate_type = 0x0e};

  encode_idt_entry(&IDT[8], dfd);
  encode_idt_entry(&IDT[14], pfd);
  encode_idt_entry(&IDT[13], general_protection_fault);
}
