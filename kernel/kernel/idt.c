#include "idt.h"

#include <kernel/io.h>
#include <kernel/pic.h>
#include <kernel/serial.h>
#include <stdint.h>
#include <stdio.h>

char scancode_to_ascii[] = {
    0,    27,   '1', '2', '3', '4', '5', '6', '7', '8', '9',  '0', '-',
    '=',  '\b',  // 0x00–0x0E
    '\t', 'q',  'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',  '[', ']',
    '\n',                                                           // 0x0F–0x1C
    0,                                                              // Control
    'a',  's',  'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',  // 0x1D–0x28
    0,                                                            // Left shift
    '\\', 'z',  'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',  0,  // 0x29–0x35
    '*',  0,                                                      // Alt
    ' ',                                                          // Space
    0,                                                            // CapsLock
    // and so on...
};

#define SCANCODE_LENGTH 58

static uint64_t IDT[256] = {0};

static int _read_cr2(void) {
  int cr2 = 0;
  __asm__ volatile("mov %%cr2, %0" : "=r"(cr2));
  return cr2;
}

struct __attribute__((packed)) idtr {
  uint16_t size;
  uint32_t offset;
};

typedef struct {
  uint32_t edi;
  uint32_t esi;
  uint32_t ebp;
  uint32_t esp_dummy;  // PUSHA/PUSHAD pushes original ESP. We don't typically
                       // use this in C.
  uint32_t ebx;
  uint32_t edx;
  uint32_t ecx;
  uint32_t eax;

  // These are pushed by our assembly stub
  uint32_t int_no;      // The interrupt number (pushed by us)
  uint32_t error_code;  // The error code (pushed by hardware for some, or dummy
                        // 0 by us)

  // These are pushed by the CPU hardware
  uint32_t eip;     // Instruction Pointer
  uint32_t cs;      // Code Segment
  uint32_t eflags;  // EFLAGS Register
} __attribute__((packed)) regs_t;

void encode_idt_entry(uint8_t *target, struct InterruptDescriptor id) {
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

void _isr8_handler(regs_t *stack_frame) {
  serialprint("[INT] number %d\n", stack_frame->int_no);
  serialprint("DOUBLE FALUT!\n");
}

void _isr13_handler(regs_t *stack_frame) {
  serialprint("[INT] number %d\n", stack_frame->int_no);
  serialprint("GENERAL PROTECTION!\n");
}

void _isr14_handler(regs_t *stack_frame) {
  serialprint("[INT] number %d\n", stack_frame->int_no);
  int cr2 = _read_cr2();
  serialprint("PAGE FAULT! Error code: %d Accessed page: 0x%x\n",
              stack_frame->error_code, cr2);
}

void _isr32_handler(regs_t *stack_frame) {
  // serialprint("[INT] number %d\n", stack_frame->int_no);
  uint8_t irq_no = stack_frame->int_no - 32;  // fix hard code values
  // serialprint("[IRQ] number %d [TIMER]\n", irq_no);
  endIOE(irq_no);
}

void _isr33_handler(regs_t *stack_frame) {
  // serialprint("[INT] number %d\n", stack_frame->int_no);
  uint8_t irq_no = stack_frame->int_no - 32;  // fix hard code values
  // serialprint("[IRQ] number %d [KEYBOARD]\n", irq_no);
  uint8_t scancode = inb(0x60);
  // serialprint("[SCANCODE] %d [CHAR] %c\n", scancode,
  // scancode_to_ascii[scancode]);
  if (scancode < SCANCODE_LENGTH) {
    printf("%c", scancode_to_ascii[scancode]);
  }
  endIOE(irq_no);
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

  extern isr32_handler(void);
  struct InterruptDescriptor isr32 = {.offset = (uint32_t)isr32_handler,
                                      .segment_selector = 0x08,
                                      .dpl = 0x00,
                                      .present = 0x01,
                                      .gate_type = 0x0e};

  extern isr33_handler(void);
  struct InterruptDescriptor isr33 = {.offset = (uint32_t)isr33_handler,
                                      .segment_selector = 0x08,
                                      .dpl = 0x00,
                                      .present = 0x01,
                                      .gate_type = 0x0e};

  encode_idt_entry(&IDT[8], dfd);
  encode_idt_entry(&IDT[14], pfd);
  encode_idt_entry(&IDT[13], general_protection_fault);
  encode_idt_entry(&IDT[32], isr32);
  encode_idt_entry(&IDT[33], isr33);
}
