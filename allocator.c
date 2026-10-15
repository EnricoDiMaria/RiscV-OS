#include "allocator.h"
#include "buddy.h"
#include "slab.h"
#include "math.h"

paddr_t alloc_pages(uint64_t n){
    return buddy_alloc_pages(power2exp(n));
}