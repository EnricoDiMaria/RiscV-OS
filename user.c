#include "user.h"

extern char __U_stack_top[];
extern void start(void);
extern void exit(void);
extern int syscall(int arg0, int arg1, int arg2, int sysno);

void printc(char ch) { 
    syscall(1, ch, 0, 0);
}