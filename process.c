#include "process.h"
#include "UART.h"
#include "plic.h"

extern void switch_context(uint64_t *prev_sp, uint64_t *next_sp);
extern void switch_page_table(uint64_t satp_value, uint64_t sscratch_value);
extern void user_switch(uint64_t user_base);

struct process {
    int pid;             // Process ID (to be used in kill(pid) for example)
    int state;           // Process state: PROC_UNUSED or PROC_RUNNABLE: used to tell whether that slot is used by a real process or not
                            // the kernel (in the struct process[PROCS_MAX]) array searchs for the first empty slot
    vaddr_t sp;          // Stack pointer: used by the kernel to save the context of a process before executing another one
    uint64_t *page_table;//pointer to the first-level page table
    uint8_t stack[8192]; // Kernel stack: to be used by every process. If there was only one stack for every process context switching would not be
                            //possible because different processes would write in the same part of memory and in case of switch back and forth between two
                            //it would be harder to keep track which part of memory belongs to a certain process
};

struct process procs[PROCS_MAX]; //array for all processes
//because the array is defined not inizialized it will be stored in the .bss section, that is inizialized at zero at the start of the 
//kernel, therefore at the boot of the OS all processes are inizialized at PROC_UNUSED (== 0)
//the array is defined in the .c to be sure that there is only one procs array across the whole system

static struct process *idle_proc;
struct process *current_proc;


void delay(int n) { //n *1000 of nop
    for (int i = 0; i < n*1000; i++)
        __asm__ __volatile__("nop"); // do nothing
}

extern char __kernel_base__[];

void user_entry(void) {
    user_switch(USER_BASE);
}

struct process *create_process(const void *image, size_t image_size) { //pointer to the execution image and image size
    struct process *proc = NULL;
    int i;
    for (i = 0; i<PROCS_MAX; i++) {
        if (procs[i].state == PROC_UNUSED){
            proc = &procs[i];
            break;
        }
    }

    if (!proc) PANIC("No free process slots! ");

    uint64_t *sp = (uint64_t *) &proc->stack[sizeof(proc->stack)]; //the start of the empty stack is the end of proc->stack (sp grows towards the bottom)
    //uint64_t because from this point on sp is handled as a pointer to 64 bit blocks, not to single bytes
    //s registers are inizialized at zero (because we are creating a new process) in this order so they will be read by the switch_context function in the opposite order
    *--sp = 0;              //padding
    *--sp = 0;              // s11
    *--sp = 0;              // s10
    *--sp = 0;              // s9
    *--sp = 0;              // s8
    *--sp = 0;              // s7
    *--sp = 0;              // s6
    *--sp = 0;              // s5
    *--sp = 0;              // s4
    *--sp = 0;              // s3
    *--sp = 0;              // s2
    *--sp = 0;              // s1
    *--sp = 0;              // s0
    //in ra is saved the pc from which this process will start executing once called
    *--sp = (uint64_t) user_entry;;              // ra


    // Map kernel pages.
    uint64_t *page_table = (uint64_t *) alloc_pages(1);
    for (paddr_t paddr = (paddr_t) __kernel_base__; paddr < (paddr_t) __ram_end__; paddr += PAGE_SIZE)
        map_page(page_table, paddr, paddr, PAGE_R | PAGE_W | PAGE_X); //kernel page mapping with virtual address == physical address and not accessible from U-mode
        //all free memory is mapped so the kernel can always access both statically allocated areas (like .text), and dynamically allocated areas managed by alloc_pages

    // Map MMIO: UART
    for (paddr_t paddr = UART0_BASE; paddr < UART0_BASE + PAGE_SIZE; paddr += PAGE_SIZE)
        map_page(page_table, paddr, paddr, PAGE_R | PAGE_W);

    // Map MMIO: PLIC (i registri che usi arrivano fino all'offset 0x201004, quindi serve più di una pagina)
    for (paddr_t paddr = PLIC_BASE; paddr < PLIC_BASE + PLIC_SIZE; paddr += PAGE_SIZE)
        map_page(page_table, paddr, paddr, PAGE_R | PAGE_W);

    // Map user pages.
    for (uint64_t off = 0; off < image_size; off += PAGE_SIZE) {
        paddr_t page = alloc_pages(1);

        //in case where the data to be copied is smaller than the page size.
        size_t remaining = image_size - off;
        size_t copy_size = PAGE_SIZE <= remaining ? PAGE_SIZE : remaining;

        // Fill and map the page.
        memcpy((void *) page, image + off, copy_size);
        map_page(page_table, USER_BASE + off, page, PAGE_U | PAGE_R | PAGE_W | PAGE_X);
    }

    proc->page_table = page_table;

    proc->pid = i + 1; //to give an ID to this process in the array
    proc->state = PROC_RUNNABLE; //the process can be now executed
    proc->sp = (vaddr_t) sp; //the sp address is saved after all sp and ra were pushed in the stack: this sp points to the start of the free stack memory of that process
    return proc;
}

void process_init(void) {
    idle_proc = create_process(NULL, (size_t) 0);
    idle_proc->pid = 0; // idle
    current_proc = idle_proc;
}

void yield(void) {
    // Search for a runnable process
    struct process *next = idle_proc;
    for (int i = 0; i < PROCS_MAX; i++) {
        struct process *proc = &procs[(current_proc->pid + i) % PROCS_MAX];
        if (proc->state == PROC_RUNNABLE && proc->pid > 0) {
            next = proc;
            break;
        }
    }

    // If there's no runnable process other than the current one, return and continue processing
    if (next == current_proc)
        return;

    // Context switch
    struct process *prev = current_proc;
    current_proc = next;

    switch_page_table(SATP_SV39 | ((uint64_t) next->page_table / PAGE_SIZE), (uint64_t) &next->stack[sizeof(next->stack)]);
    switch_context(&prev->sp, &next->sp);
}