.extern __ram_start__
.extern __ram_size__
.extern __ram_end__
.extern __global_pointer$
.extern __trap_stack_top__
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

la t0, __trap_stack_top__
csrw sscratch, t0

la sp, __stack_top__
andi sp, sp, -16 #to force 16 byte alignment

li t0, 0xffff
#these instructions are used to tell the CPU that interrupts and exceptions need to be handled in S-mode
csrw medeleg, t0 #machine exception delegation
csrw mideleg, t0 #machine interrupt delegation

li t0, 0x1f
csrw pmpcfg0, t0 #to grant S-mode Read, Write and Execution on all memory
li t0, -1 
csrw pmpaddr0, t0 #with t0 all ones we tell the CPU that S-mode can access the whole ram memory

li t0, (0b01 << 11) | (1<<5) | (1<<1)#the kernel will be executed in S-mode (01 in 11) and SPIE and SIE are activated
csrw mstatus, t0

csrw satp, zero #virtual memory is for now disabled

li t0, (1 << 9) #to enable External Interrupts in S-mode
csrw sie, t0

la t0, kernel_main
csrw mepc, t0

la t0, exception_handler
csrw stvec, t0 #exception handler in S-mode
mret #to call main_kernel loaded in mepc

error: j error #infinite loop
