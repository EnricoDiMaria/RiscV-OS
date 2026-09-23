#include "user.h"

void printc(char ch) { 
    syscall(SYS_PRINTC, ch, 0, 0);
}

int getc(void) {
    return syscall(SYS_GETC, 0, 0, 0);
}

void yield(void) {
    syscall(SYS_YIELD, 0, 0, 0);
}