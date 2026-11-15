#include "buddy.h"
#include "common.h"

extern uint8_t __ram_start__[]; 
extern uint8_t __ram_end__[];
extern uint8_t __free_ram__[];

#define BUDDY_MAX_ORDER 15 //2^15 gives 32768 (number of pages in this system): for the buddy allocator this number means the finest subdivision of memory, rapresented by the leafs of the tree
#define BUDDY_PAGE_COUNT (1ULL << BUDDY_MAX_ORDER) //to calculate 2^15
#define BUDDY_BITMAP_BYTES ((2 * BUDDY_PAGE_COUNT + 7) / 8) //it calculates total nodes count, round up that number and expresses it in bytes (/8)

static uint8_t buddy_bitmap[BUDDY_BITMAP_BYTES];//buddy map is placed in the .bss section, before __free_ram__ (static allocation of the bitmap)


struct free_block { //it is saved in the start of each free block and will get overwritten when the block is used (no memory is wasted for headers)
    struct free_block *prev;
    struct free_block *next;
};

static struct free_block *free_list[BUDDY_MAX_ORDER + 1];// to quickly find a free block of a certain dimension (per order). E.g. free_list[1] points to the first free block with that size

#define PAGE_INDEX(x) (((paddr_t)(x) - (paddr_t)__ram_start__) / PAGE_SIZE)
#define INDEX(x) ((x)/8)
#define BIT(x) ((x)%8)
#define BUDDY(x) ((x)^1)

static void bitmap_set(uint64_t n){ //a block set to 1 is usable
    int index = INDEX(n);
    int offset = BIT(n);
    buddy_bitmap[index] |= 1<<offset;
} 

static void bitmap_clear(uint64_t n) {
    int index = INDEX(n);
    int offset = BIT(n);
    buddy_bitmap[index] &= ~(1u << offset); // creates a bitmask with a 0 at 'offset' and 1 elsewhere, then uses bitwise AND to clear only that specific bit
}

static uint8_t bitmap_get(uint64_t n) {
    int index = INDEX(n);
    int offset = BIT(n);
    return (buddy_bitmap[index] >> offset) & 1;
}

void buddy_init(void) {
    //memset(buddy_bitmap, 0, sizeof(buddy_bitmap)); //it could be safer to explicitly set these sections to zero, but it is actually already done by the inizilization to 0 of the .bss section
    //memset(free_list, 0, sizeof(free_list));

    paddr_t ram_end = (paddr_t)__ram_end__;
    paddr_t current = (paddr_t)__free_ram__;

    while (current < ram_end) { //partition the whole free physical RAM into the largest possible aligned blocks
        size_t remaining_pages = (ram_end - current) / PAGE_SIZE;

        unsigned order = BUDDY_MAX_ORDER;

        while (order > 0) { //greedily find the largest order that fits and satisfies natural buddy alignment
            size_t block_pages = 1ULL << order;

            //a block of 2^order pages must fit in the remaining space and its starting page index must be a multiple of 2^order
            if (block_pages <= remaining_pages && (PAGE_INDEX(current) & (block_pages - 1)) == 0)
                break;
            order--;
        }

        free_list_add(current, order); //register the block in the corresponding order's free list

        current += (1ULL << order) * PAGE_SIZE; //advance cursor past the newly added block
    }
}

static size_t node_index(paddr_t paddr, unsigned order) { //to find the index of the node corresponding to a certain address of memory (given the order)
    return (BUDDY_PAGE_COUNT >> order) + (PAGE_INDEX(paddr) >> order); //(start of the corresponding level) + (number of blocks before the one searched for: in every order block there are 2^order pages)
}

static void free_list_add(paddr_t paddr, unsigned order) { //insert a free block in the list of its order (insertion at the beginnning)
    struct free_block *block = (struct free_block *)paddr; // with this cast we can use the first bytes as prev and next

    block->prev = NULL;
    block->next = free_list[order];

    if (block->next != NULL)
        block->next->prev = block;

    free_list[order] = block;

    bitmap_set(node_index(paddr, order));
}


paddr_t buddy_alloc_pages(uint64_t order){  //allocates a number of pages equal to 2^order


}
