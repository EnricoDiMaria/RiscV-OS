#ifndef virtual_h
#define virtual_h

#include "common.h"
#include "kernel.h"

#define SATP_SV39 ((uint64_t)8 << 60) //in RISC-V 64 the last 4 bits in the sapt register are used to indicate the mode of paging: 8 is SV39
#define PAGE_V    (1 << 0)   // "Valid" bit (entry is enabled)
#define PAGE_R    (1 << 1)   // Readable
#define PAGE_W    (1 << 2)   // Writable
#define PAGE_X    (1 << 3)   // Executable
#define PAGE_U    (1 << 4)   // User (accessible in user mode)

void map_page(uint64_t *table2, uint64_t vaddr, paddr_t paddr, uint64_t flags);

#endif