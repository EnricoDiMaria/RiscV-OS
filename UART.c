#include "UART.h"

void UART_init() {
    UART->IER = 0x01; //to enable receiver data interrupt (transmitting will be enabled each time when needed)
    UART->LCR = (0 << 7)| 0x03; //word length set to 8 bit and 0 to DLAB register
    UART->FCR_IIR = 0x01; //to enable FIFO buffer
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

int UART_rReady() {
    return (UART->LSR & 1);
}

int UART_interrupt_id() {
    return (UART->FCR_IIR & 0b1110) >> 1;
}
