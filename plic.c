#include "plic.h"
#include "UART.h"

#define PLIC_BASE 0x0c000000 //PLIC is MMIO
#define PLIC_PRIORITY ((volatile uint32_t *)(PLIC_BASE + 0x0000)) //to define the priority of a certain interrupt
#define PLIC_ENABLE   ((volatile uint32_t *)(PLIC_BASE + 0x2000)) //to enable certain interrupt or not in M-mode
#define PLIC_THRESHOLD ((volatile uint32_t *)(PLIC_BASE + 0x200000)) //to define the threshold (an interrupt with a priority under the value of
//the value defined here will be ignored)
#define PLIC_CLAIM    ((volatile uint32_t *)(PLIC_BASE + 0x200004)) //this register serves two functions:
//if read it stores the id of the device that sent the interrupt
//if written it tells the PLIC module that the interrupt of that ID has been processed

static void enable_module(int ID) {
    PLIC_PRIORITY[ID] = 1;
    *PLIC_ENABLE = (1<<ID);
}

uint32_t PLIC_interrupt_request() {
    return *PLIC_CLAIM;
}

void PLIC_interrupt_end(uint32_t ir) {
    *PLIC_CLAIM = ir;
    return;
}

void PLIC_enable_interrupts_modules() {
    *PLIC_THRESHOLD = 0; //with 0 every interrupt with priority >0 is processed
    //UART
    enable_module(10); // UART is module 10
    UART_init();
}