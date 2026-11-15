#ifndef allocator_h
#define allocator_h

#include "common.h"

//defined using char to work in bytes and have the location of the single byte
extern uint8_t __ram_start__[];
extern uint8_t __ram_size__[]; //the array decays to a pointer to the start of this section
extern uint8_t __ram_end__[];
extern uint8_t __stack_top__[];
extern uint8_t __bss_start__[]; //"the value at the 0th byte of the .bss section"
extern uint8_t __bss_end__[];
extern uint8_t __free_ram__[];

extern PAGE_SIZE; //to make it possible for virtual to read this variable but not other things from buddy
paddr_t alloc_pages(uint64_t n);
void allocator_init(void);

#endif