#ifndef kfunctions_h
#define kfunctions_h

#include "common.h"

//defined using char to work in bytes and have the location of the single byte
extern uint8_t __ram_start__[]; //the array decays to a pointer to the start of this section
extern uint8_t __ram_size__[];
extern uint8_t __ram_end__[];
extern uint8_t __stack_top__[];
extern uint8_t __bss_start__[]; //"the value at the 0th byte of the .bss section"
extern uint8_t __bss_end__[];
extern uint8_t __free_ram__[];

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
void set_sepc(uint64_t val);
void printc(char ch);
int getc(void);

#endif