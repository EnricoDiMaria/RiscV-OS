#ifndef buddy_h
#define buddy_h

#define PAGE_SIZE 4096

void buddy_init();
paddr_t buddy_alloc_pages(uint64_t order);
void buddy_free_pages(paddr_t paddr, uint64_t order);

#endif