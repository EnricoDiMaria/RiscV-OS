#ifndef plic_h
#define plic_h

#include "common.h"

#define PLIC_BASE 0x0c000000
#define PLIC_SIZE 0x400000 

void PLIC_enable_interrupts_modules();
uint32_t PLIC_interrupt_request();
void PLIC_interrupt_end(uint32_t ir);

#endif
