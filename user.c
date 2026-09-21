#include "user.h"

void printc(char ch) { 
    syscall(1, ch, 0, 0);
}