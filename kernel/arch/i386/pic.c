#include <kernel/io.h>
#include <stdint.h>

// master and slave address
#define MASTER_PIC 0x20
#define MASTER_PIC_COMMAND MASTER_PIC
#define MASTER_PIC_DATA (MASTER_PIC + 1)
#define SLAVE_PIC 0xA0
#define SLAVE_PIC_COMMAND SLAVE_PIC
#define SLAVE_PIC_DATA (SLAVE_PIC + 1)

// intialization flags
#define ICW1_ICW4 0x01
#define ICW1_SINGLE 0x02
#define ICW1_INTERVAL4 0x04
#define ICW1_LEVEL 0x08
#define ICW1_INIT 0x10

#define ICW4_8086 0x01
#define ICW4_AUTO 0x02
#define ICW4_BUF_SLAVE 0x08
#define ICW4_BUF_MASTER 0x0C
#define ICW4_SFNM 0x10

#define CASCADE_IRQ 2

// EOI end of interrupt
#define EOI 0x20
void endIOE(uint8_t irq) {
  if (irq > 7) {
    outb(SLAVE_PIC_COMMAND, EOI);
  }
  outb(MASTER_PIC_COMMAND, EOI);
}

void pic_initialize(uint32_t offset1, uint32_t offset2) {
  // initial command
  outb(MASTER_PIC_COMMAND, ICW1_INIT | ICW1_ICW4);
  io_wait();
  outb(SLAVE_PIC_COMMAND, ICW1_INIT | ICW1_ICW4);
  io_wait();

  // vector offset
  // why send int as char?
  outb(MASTER_PIC_DATA, offset1);
  io_wait();
  outb(SLAVE_PIC_DATA, offset2);
  io_wait();

  // wiring
  outb(MASTER_PIC_DATA, 1 << CASCADE_IRQ);
  io_wait();
  outb(SLAVE_PIC_DATA, 2);
  io_wait();

  // ICW4
  outb(MASTER_PIC_DATA, ICW4_8086);
  io_wait();
  outb(SLAVE_PIC_DATA, ICW4_8086);
  io_wait();

  // unmask
  outb(MASTER_PIC_DATA, 0);
  outb(SLAVE_PIC_DATA, 0);
}
