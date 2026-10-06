#include "common.h"


//defined using char to work in bytes and have the location of the single byte
extern uint8_t __ram_start__[]; //the array decays to a pointer to the start of this section
extern uint8_t __ram_size__[];
extern uint8_t __ram_end__[];
extern uint8_t __stack_top__[];
extern uint8_t __bss_start__[]; //"the value at the 0th byte of the .bss section"
extern uint8_t __bss_end__[];
extern uint8_t __free_ram__[];

#define PAGE_SIZE 4096
size_t max_pages = 0;


void buddy_init() {
    max_pages = (__ram_end__ - __free_ram__) / PAGE_SIZE;

}


paddr_t alloc_pages(uint64_t order){  //allocates a number of pages equal to 2^order


}

void free_pages(paddr_t paddr, uint64_t order);

