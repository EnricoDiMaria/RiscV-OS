#include "virtual.h"

void map_page(uint64_t *table2, uint64_t vaddr, paddr_t paddr, uint64_t flags) { //given the page table, this function creates a correlation between the virtual and physical address to be used later
    if (!is_aligned(vaddr, PAGE_SIZE))
        PANIC("unaligned vaddr %x", vaddr);

    if (!is_aligned(paddr, PAGE_SIZE))
        PANIC("unaligned paddr %x", paddr);

    //1st level pointed by satp
    uint64_t vpn2 = (vaddr >> 30) & 0x1FF; //page entries in sv39 are 8 bytes, so in a 4kb pages there are 512 entries = 2^9
    if ((table2[vpn2] & PAGE_V) == 0) {
        //create the 1st level page table if it doesn't exist
        uint64_t pt_paddr = alloc_pages(1);
        table2[vpn2] = ((pt_paddr / PAGE_SIZE) << 10) | PAGE_V; 
    }
    //2nd level pointed by the value read in table2
    uint64_t *table1 = (uint64_t *) ((table2[vpn2] >> 10) * PAGE_SIZE); //to not read the flags: what remains is the Physical Page Number (then multiplied for the page size)
    uint64_t vpn1 = (vaddr >> 21) & 0x1FF;

    if ((table1[vpn1] & PAGE_V) == 0) {
        uint64_t pt_paddr = alloc_pages(1);
        table1[vpn1] = ((pt_paddr / PAGE_SIZE) << 10) | PAGE_V;
    }

    //3rd level pointed by the value read in table 1
    uint64_t *table0 = (uint64_t *) ((table1[vpn1] >> 10) * PAGE_SIZE);
    uint64_t vpn0 = (vaddr >> 12) & 0x1FF;
    table0[vpn0] = ((paddr / PAGE_SIZE) << 10) | flags | PAGE_V;
}