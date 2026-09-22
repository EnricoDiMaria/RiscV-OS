#include "user.h"

void printc(char ch) { 
    syscall(SYS_PRINTC, ch, 0, 0);
}

long getc(void) {
    syscall(SYS_GETC, 0, 0, 0);
}