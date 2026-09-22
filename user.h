#ifndef user_h
#define user_h

#include "common.h"

#define SYS_PRINTC 1
#define SYS_GETC 2

extern char __U_stack_top[];
extern void start(void);
extern void exit(void);
extern int syscall(int sysno, int arg0, int arg1, int arg2);

#endif