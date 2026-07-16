#include "UART.h"

void UART_init() {
    UART->IER = 0x03; //to enable bot read and write
    UART->LCR = 0x03; //word length set to 8 bit
    UART->FCR = 0x01; //to enable FIFO buffer
}

void UART_reset() {
    UART->IER = 0x00;
}

void UART_enable() {
    UART->IER = 0x03; //to enable back both read and write
}

int UART_wReady() {
    return ((UART->LSR & 0b100000) >> 5);
}
