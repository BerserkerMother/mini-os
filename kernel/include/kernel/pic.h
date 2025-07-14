#ifndef PIC_H
#define PIC_H

#include <stdint.h> 

void pic_initialize(uint32_t offset1, uint32_t offset2);
void endIOE(uint8_t irq);

#endif
