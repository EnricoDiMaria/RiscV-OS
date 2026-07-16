#include "kernel.h"
#include "common.h"
#include "plic.h"
#include "UART.h"

void kernel_main(void) {
    memset(&__bss_start__, (char) 0, (size_t) (&__bss_end__ - &__bss_start__));
    //without & C would evaluate the variable (reading what it's stored at the place where the __bss_start__ label is)
    //with & C does not read in RAM the value but it uses the address of the label 
    PLIC_enable_interrupts_modules();

    printc("Hello World! RISC-V kernel in development...\n");
    printf("Test numero %d\n", 1);

    //PANIC("Something went wrong here... ");
    //printf("If you read this then something went even more wrong!");

    
    for (;;); //infinite loop

}

void trap_handler(uint64_t mcause, uint64_t mtval, uint64_t mepc) {
    switch(((mcause & 0x8000000000000000ULL) >> 63)) { //to check whether is interrupt or not (64th bit of mcause)
        case(0): //synchronous exception
        
        break;

        case(1): //external interrupt

        if ((mcause & 0xFF) == 11) { //then it's Machine External Interrupt (PLIC)
            uint32_t ir = PLIC_interrupt_request();

            if (ir == 10 && UART_wReady()) {
                if(!uart_done()) UART->THR = buffer_next();
            }
        
            PLIC_interrupt_end(ir);
        }
        break;

        default: //panic
    }

//PANIC to be implemented
}
