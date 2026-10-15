#include "buddy.h"
#include "common.h"

//defined using char to work in bytes and have the location of the single byte
extern uint8_t __ram_start__[]; //the array decays to a pointer to the start of this section
extern uint8_t __ram_size__[];
extern uint8_t __ram_end__[];
extern uint8_t __stack_top__[];
extern uint8_t __bss_start__[]; //"the value at the 0th byte of the .bss section"
extern uint8_t __bss_end__[];
extern uint8_t __free_ram__[];

#define FATHER(x) ((x)/2)
#define LEFT(x) (2*(x))
#define RIGHT(x) (2*(x)+1)
#define BUDDY(x) ((x)^1)
#define FIRST 

size_t max_pages = 0;
uint8_t *buddy_bitmap;


void buddy_init() {
    max_pages = (__ram_end__ - __ram_start__) / PAGE_SIZE; //to have a power of 2 number of pages I start the tree from ram_start and mark as occupied all the pages until free_ram
    buddy_bitmap = malloc(1*((max_pages*2)/8)); //it's 2 bits per page (divided by eight because it's in byte so I need a byte every 8 pages)

}


paddr_t buddy_alloc_pages(uint64_t order){  //allocates a number of pages equal to 2^order


}

void buddy_free_pages(paddr_t paddr, uint64_t order);

