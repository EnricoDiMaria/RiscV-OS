#include "UART.h"

void UART_init() {
    UART->IER = 0x00; //to enable bot read and write
    UART->LCR = 0x03; //word length set to 8 bit
    UART->FCR = 0x01; //to enable FIFO buffer
}

void UART_reset_o() {
    UART->IER &= ~(1 << 1);
    return;
}

void UART_enable_o() {
    UART->IER |= (1 << 1);
    return;
}

int UART_o_enabled() {
    return (UART->IER & (1 << 1));
}

int UART_wReady() {
    return ((UART->LSR & 0b100000) >> 5);
}
