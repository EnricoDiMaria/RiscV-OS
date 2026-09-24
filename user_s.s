.extern __U_stack_top

.section .text.start
.globl start
start:
la sp, __U_stack_top
call main
call exit

#.globl exit
#exit:
inf_loop:
j inf_loop

.globl syscall
syscall:
# a0 = sysno
# a1 = arg0
# a2 = arg1
# a3 = arg2
ecall
ret