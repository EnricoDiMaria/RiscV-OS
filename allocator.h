#ifndef allocator_h
#define allocator_h

#include "common.h"

extern PAGE_SIZE; //to make it possible for virtual to read this variable but not other things from buddy
paddr_t alloc_pages(uint64_t n);

#endif