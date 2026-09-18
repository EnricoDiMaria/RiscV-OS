.text
.align 2
.globl switch_context

# Context switch between two processes: saves the callee-saved registers of the
# CURRENT process onto its own stack (pointed to by the current sp), then stores
# that sp into *prev_sp so it can be restored later. It then swaps sp to point to
# the NEXT process's stack (read from *next_sp), and restores the callee-saved
# registers from THAT stack instead — these are the values the next process had
# saved the last time IT was switched out. Because sp now points to a different
# stack, the final `ret` doesn't return to this function's caller: it jumps to
# whatever return address (ra) was saved on the next process's stack, resuming
# that process exactly where it left off. Only callee-saved registers need to be
# saved/restored here, since the caller (per the RISC-V calling convention) is
# already responsible for preserving any caller-saved registers it needs across
# this function call.

switch_context: #a0 = uint64_t *prev_sp, a1 = uint64_t *next_sp

addi sp, sp, -8*14 #only calle-saved registers has to be saved
#ABI RV64 requires stack pointer always kept 16-byte aligned (14*8=112 and 112 mod 16 = 0)
sd ra, 0*8(sp)
sd s0, 1*8(sp)
sd s1, 2*8(sp)
sd s2, 3*8(sp)
sd s3, 4*8(sp)
sd s4, 5*8(sp)
sd s5, 6*8(sp)
sd s6, 7*8(sp)
sd s7, 8*8(sp)
sd s8, 9*8(sp)
sd s9, 10*8(sp)
sd s10, 11*8(sp)
sd s11, 12*8(sp)
#13 is padding

sd sp, (a0) #a0 contains the address of prev_sp so (a0) deferences that address and the current sp value is stored at the address
            #pointed by prev_sp
ld sp, (a1) #a1 contains the address of next_sp so (a1) deferences that address and the values stored at that address is loaded into sp

ld ra, 0*8(sp)
ld s0, 1*8(sp)
ld s1, 2*8(sp)
ld s2, 3*8(sp)
ld s3, 4*8(sp)
ld s4, 5*8(sp)
ld s5, 6*8(sp)
ld s6, 7*8(sp)
ld s7, 8*8(sp)
ld s8, 9*8(sp)
ld s9, 10*8(sp)
ld s10, 11*8(sp)
ld s11, 12*8(sp)

addi sp, sp, 8*14

ret

.globl switch_page_table
switch_page_table:
sfence.vma
csrw satp, a0
csrw sscratch, a1
sfence.vma
ret

.globl user_switch
user_switch:
csrw sepc, a0
li t0, (1<<5)
csrw sstatus, t0 #SSTATUS_SPIE -> it enables hardware interrupts in U-mode
sret
