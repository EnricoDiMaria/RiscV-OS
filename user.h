#ifndef user_h
#define user_h

#include "common.h"

extern char __U_stack_top[];
extern void start(void);
extern int syscall(int sysno, int arg0, int arg1, int arg2);
void printc(char ch);
int getc(void);
void yield(void);
void exit(void);

#endif