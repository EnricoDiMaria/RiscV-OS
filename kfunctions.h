#ifndef kfunctions_h
#define kfunctions_h

#include "common.h"

#define USER_BASE 0x1000000UL

int buffer_is_empty();
int buffer_is_full();
void buffer_add(char c);
uint8_t buffer_next();
void rbx_add(char ch);
int rbx_is_full();
void rbx_add(char ch) ;
int rbx_next();

paddr_t alloc_pages(uint64_t n);

void panic_printf(const char *str, ...);
void printc(char ch);
int getc(void);

#endif