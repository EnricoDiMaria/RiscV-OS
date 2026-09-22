#ifndef uart_h
#define uart_h

#include "common.h"

#define UART0_BASE 0x10000000L


typedef struct __attribute__((packed)){
    volatile uint8_t THR_RBR; //first register of UART is the data register that holds the character that has to be written or the character read
    volatile uint8_t IER; //interrupt enable
    volatile uint8_t FCR_IIR; // FIFO Control Register and Interrupt Identification Register
    volatile uint8_t LCR; // Line Control Register
    volatile uint8_t MCR; // Modem Control Register
    volatile uint8_t LSR; // Line Status Register
}UART_hardware_t; //a struct is overlapped to the physical memory (in C the variables in the struct are stored in contigous memory)

#define UART ((UART_hardware_t *) UART0_BASE)

#define UART_IID_TX_EMPTY 1
#define UART_IID_RX_AVAIL 2

void UART_init();
void UART_enable_o();
void UART_reset_o();
int UART_wReady();
int UART_o_enabled();
int UART_rReady();
int UART_interrupt_id();

#endif