#include "kernel.h"
#include "kfunctions.h"
#include "common.h"
#include "plic.h"
#include "UART.h"
#include "process.h"

/*temporary test payload: will become real user processes*/
/* struct process *proc_a, *proc_b;

void proc_a_entry(void) {
    printf("starting process A\n");
    while (1) {
        delay(30000);
        printc("ProcessA ");
        yield();
    }
}

void proc_b_entry(void) {
    printf("starting process B\n");
    while (1) {
        delay(30000);
        printc("ProcessB ");
        yield();
    }
}
*/

extern char _binary_shell_bin_start[], _binary_shell_bin_size[];

void kernel_main(void) {
    memset(__bss_start__, (char) 0, (size_t) (__bss_end__ - __bss_start__));
    PLIC_enable_interrupts_modules();

    printf("Hello World! RISC-V kernel in development...\n");
    printf("Test numero %d\n", 2);

    process_init();
    
    //proc_a = create_process((uint64_t) proc_a_entry, sizeA);
    //proc_b = create_process((uint64_t) proc_b_entry, sizeB);
    
    create_process(_binary_shell_bin_start, (size_t) _binary_shell_bin_size);
    
    yield();
    PANIC("switched to idle process");
    
   

    //PANIC("Something went wrong here... ");
    //printf("If you read this then something went even more wrong!");

    //paddr_t paddr0 = alloc_pages(2);
    //paddr_t paddr1 = alloc_pages(1);
    //printf("alloc_pages test: paddr0=%x\n", paddr0);
    //printf("alloc_pages test: paddr1=%x\n", paddr1);
    //in order to be sure that paddr0 is actually set to the start of free ram the following command can be used:
    //llvm-nm kernel.elf | grep __free_ram__

    

    
    for (;;); //infinite loop

}

void syscall_handler(struct trap_frame *f) {
    switch (f->a0) {
        case 1: //SYS_PUTCHAR
            printc((char) f->a1);
            break;
        default:
            PANIC("unexpected syscall a0=%x\n", f->a0);
    }
}

void trap_handler(struct trap_frame *f, uint64_t scause, uint64_t stval, uint64_t sepc) {
    switch(((scause & 0x8000000000000000ULL) >> 63)) { //to check whether is interrupt or not (64th bit of mcause)
        case(0): //exception
        switch (scause & 0xFF) {
            case (2): //Illegal instruction
                PANIC("illegal instruction (addr=%x)\n", stval);
                break;
            case(7): //Store/AMO access fault
                if ((paddr_t) stval >= (paddr_t) __ram_end__) PANIC("out of memory: allocator ran past __ram_end__ (addr=%x)\n", stval);
                else PANIC("store access fault: write to unmapped address=%x, sepc=%x\n", stval, sepc);
                break;
            case(8): //SCAUSE_ECALL in USER MODE
                syscall_handler(f);
                set_sepc(sepc+4);
                break;
            

            default: PANIC("synchronous exception scause=%x, stval=%x, sepc=%x\n", scause, stval, sepc);
            


        }
        break;

        case(1): //external interrupt
        switch (scause & 0xFF) {
            case(9): //then it's Supervisor External Interrupt
                uint32_t ir = PLIC_interrupt_request();

                if (ir == 10) {
                if (!buffer_is_empty()) UART->THR = buffer_next();
                else UART_reset_o();
                }
        
                PLIC_interrupt_end(ir);
                break;
            
            default: PANIC("external exception scause=%x, stval=%x, sepc=%x\n", scause, stval, sepc);
        
    }
        break;

        default: PANIC("unexpected trap scause=%x, stval=%x, sepc=%x\n", scause, stval, sepc);
    }
}

