.extern __ram_start__
.extern __ram_size__
.extern __ram_end__
.extern __stack_top__

.extern exception_handler

.section .init, "ax"
.globl __start__
__start__: #a RISCV system will execute automatically the code at the start of the ram
            #so i just need to save the stack pointer
.option push
.option norelax
la gp, __global_pointer$
.option pop

la sp, __stack_top__
andi sp, sp, -16 #to force 16 byte alignment

li t0, (0b11 << 11) | (1<<7) | (1<<3) #the kernel will be executed in M-mode 
csrw mstatus, t0

li t0, (1 << 11) #to enable Machine External Interrupts
csrw mie, t0

la t0, kernel_main
csrw mepc, t0

la t0, exception_handler
csrw mtvec, t0 #exception handler in M-mode
mret #to call main_kernel loaded in mepc

error: j error #infinite loop
