# Writing a Bootloader/Kernel/OS in RISC-V 64 bit from scratch

First of all we need to write the `memory_map.ld` to tell the compiler where to put the start of the `.text` code from the bootloader (`boot.s`). The memory_map will be later included in the `linker.ld`.

Since we want to run the OS on qemu-virt, from QEMU documentation we can see that the virt ram starts at address `0x80000000`. The virt machine uses a big ram memory to store everything (bootloader, kernel, datas), hence we just declare this memory section and not separate ones.

In `linker.ld` we declare sections to decide where and in what order to put the compiled program in memory.

Now it's time to start writing the bootloader itself. I decided to write it directly in assembly. First of all I need to access the variables declared in the linker. Then I write the procedure called `__start__` (same name as in the linker), where I save the stack pointer (needed to execute any C function), I load `mtvec` with an assembly exception handler address and I call the kernel (in M-mode for now).

In the kernel the first function implemented is `memset` (to set a specific section of memory to a specified value). The function is used right after calling the kernel to initialize at zero the `.bss` section.

A Makefile is provided to compile and run the OS with a single line in the terminal (`make run` to run, `make clean` to clean). To access the qemu console the shortcut is `Ctrl+A` followed by `C` (console). To get info about registers the `info registers` command can be used: from this section it can be easily seen that the PC is correctly set to a value that is the offset from the ram start address (`0x80000000`) plus the `kernel_main` function address (at the point of execution). This can be easily verified by disassembling the executable with `riscv64-elf-objdump -d kernel.elf | less`.

To implement a console output/input we will use UART (Universal Asynchronous Receiver / Transmitter), emulated by QEMU, that works by writing at the `0x10000000` address (it's Memory Mapped I/O). When we write something to this address (or an offset of this address) we are actually communicating with the UART device. Reading from a pointer pointing to that address extracts 8 bits from the receiver (RBR), while writing to said pointer transmits 8 bits to the transmitter (THR).

Even though QEMU is virtualized so the output from the UART register to the console is almost immediate, I decided to implement a check on the Line Status Register (LSR) that is how real hardware would tell us if the character has been sent. I want to implement a function (`printc`) to print a string.

To do so I need to implement first a Ring Buffer (FIFO), from which the interrupt handler will read every character in order. We implement the FIFO with a circular buffer with head (to write) and tail (to read) pointers.

The exception handler is now empty: the first thing to implement is saving all registers and passing `mcause`, `mtval` and `mepc` as parameters to the `trap_handler` C function. The PLIC and UART modules are also implemented (further explanations on how they work are in the respective `.c` and `.h` files).

At the start of the kernel, `PLIC_enable_interrupts_modules` is called to enable and set priority of the modules specified in that function.

The `printc` function works by saving the string in the buffer, then it checks if the UART was already writing something: if it was, then it will automatically get to the end of the buffer and print the new string; if it wasn't, then `printc` puts the first character in the receiver register of the UART. When a new character is written in THR then it is sent in output and then the UART module sends an interrupt. I use this interrupt to finish writing the rest of the buffer: the exception handler, when called by said interrupt, understands that it was the UART calling and, if the UART is ready (it sent the interrupt so it is already ready, the control on LSR is just to be sure) and the buffer has not already been cleared, then it sends the next character to the THR and the loop goes on like that until the end of the buffer, when UART interrupts are disabled.

The `printc` function and the buffer are now moved to a `common.c`/`common.h` file in order to make them accessible from every program, not only the kernel. In this file I also include the structures. I need to do so also to implement a `printf` function (that can support format specifiers and variables). My `printf` implementation works with a switch case: when a `%` is read, it gets the correct value and the string is progressively written in the buffer; only at the end will the first character be sent to the UART that will start outputting the string.

Next I implement some standard C functions that will be useful in the OS.

Next I want to make the kernel work in S-mode. So I change `boot.s` (changes better explained in the file) and `trap.s`. In the latter it's important to save in `sp` the pointer saved by the kernel, when an exception is called, in `sscratch`: when an interrupt/exception is called while a program is being executed, `sp` will point to the section of memory of that program, while `sscratch` will point to an emergency stack. We want to use that emergency stack so as not to overwrite the section of memory used by the user, but later the old pointer needs to be restored in `sp`. In `plic.c` I need to use different addresses.

Now I implement the `PANIC` function, to be used when a Kernel Panic occurs. I cannot use `printf` because in some scenarios when an exception occurs the CPU sets SIE to zero and so `printf` cannot call other interrupts. I need to print the PANIC message with a polling approach.

In order to be able to allocate memory i need to add a `free ram` section to my linker.ld. In the portion of memory starting at the free ram pointer I can allocate n pages with the `alloc_pages` function, while moving accordingly the free ram pointer to the next free area. Because I decided to not implement a `free ram end` pointer in case the `next_paddr` address is out of bound of memory then an `Access Fault` exception would be raised and the error would be handled by the `trap_handler` (because an MMU is yet to be implemented an actual `Access Fault` from the PMA is raised).

Next I want to make my kernel work with differents threads: i need to implement a Process Control Block (PCB), a data structure that keeps track of every process in execution (ID, area of memory used...). The switch of a process happens via an interrupt (called by a timer or an event for example), that calls a switch_contest functions that saves the current sp of the process in execution and loads the sp of the process called, that gets executed from the point it was first blocked. The switch context function is written directly in assembly to not make C handle automatically the stack (in an assembly program what is written is exactly what will be generated by the compiler and so I can work directly with memory the way I want to, while in a C program the compiler would add a prologue and epilogue automatically to save local variables).
To initialize a new process I need a create_process function that that takes the entry point (the program counter) as a parameter, so the process struct will save that address so when called will start to execute code from there, and returns a pointer to the new process struct.

If I declared the struct process (for example A e B) in kernel.c those processes would have run on the boot stack because I used to call the process directly as a function in the kernel. Because I don't want that I need an idle process, that deliberately runs on the boot stack, that can create new processes and call them. Because the call of a new process is hanfled by a scheduler that loads the correct registers and then calls the switch context, every process is actually called with the ret assebly preudoinstruction (whether it's a new process just created or the process has been interrupted previously: the switch context does not care about that, it works the same way). The importance in starting/switching to a process via the switch context function is that it loads the 8kb of stack alredy destined to said process (otherwise the process would have used the boot stack). To correctly call another process (not a particular one, I just need to tell the CPU that the process I was executing can be suspended and another one can be called) I implement a yield function that check if there is another process runnable and in that case calls the switch context function. The yield function scans all the processes in the array starting from the one that just called it (the process in execution in stored in a global variable). The scheduling is cooperative: yield runs only when someone calls it.
The switch_context function needs to be updated because before it was saving the context onto the stack, while now I want to use the portion of memory dedicated to every process.
I also have to update trap.s because now at the moment of a trap sp already points to a valid kernel stack, the one of the process in execution so I don't need to swap it with sscratch to use the emergency kernel anymore. 
The page table of a process is stored in the satp register, that I need to overwrite at avery context switch. The `sfence.vma` function invalidates the TLB, because the page table is modified in this moment so the old translations could be wrong now. The first `sfence.vma` makes sure that all the operation that would have used the old TLB have been completed and it empties the old TLB. The second `sfence.vma` invalidates again the TLB so new translation would use the new page table in `satp`. The TLB will be naturally filled by new translation when needed afterwards.


**Bug: processes stop switching after enabling paging**

Once `switch_page_table()` writes `satp` for the first time (inside `yield()`), the MMU translates every memory access the kernel makes — including code fetches. But the per-process page table only identity-maps `[__kernel_base__, __ram_end__)`, i.e. physical RAM. UART (`0x10000000`) and PLIC (`0x0c000000`) are memory-mapped I/O, physically outside that range, so they're never mapped.

The moment a process tries to print (`printc`/`printf` → UART) or the trap handler services an external interrupt (`PLIC_interrupt_request`/`_end`), that access faults — because the physical address simply isn't translated by the active page table. Since `PANIC` itself calls `panic_printf`, which also writes to UART, the fault handler's own attempt to report the fault can fault again, which is why the system appears to just hang instead of cleanly switching processes or printing a clear panic message.

**Fix:** identity-map the MMIO regions the kernel actually uses, in every process's page table, with `PAGE_R | PAGE_W` (no `PAGE_X` — it's device memory, not code; no `PAGE_U` — kernel-only).

1. Expose `PLIC_BASE` (currently private to `plic.c`) in `plic.h`, plus a `PLIC_SIZE` (e.g. `0x400000`, enough to cover priority/enable/threshold/claim registers, which are sparse across ~2MB of address space).
2. In `create_process`, after mapping the kernel RAM range, add two more identity-map loops: one page for `UART0_BASE`, and `PLIC_SIZE` bytes starting at `PLIC_BASE`.

This should restore printing and interrupt handling once paging is active, so process switching resumes as before.


`Page Table`
In this kernel I use RISC-V's paging mechanisms called Sv39, so I can address 512Gb because this implementation supports a 39-bit virtual address space, divided into 4 Kib pages. [Insert figure 63 Risc-V spec]. Because the instructions effectively use a 64 bit address this paging mechanism must have bits 63-39 all equal to bit 38 (VPN[2] is extended). The paging mechanism works by using the top 27 bits of the 39 bits to index into a page table to find the physical address, whose bottom 12 bits are copied from the original virtual address (page offset). The section of bits is divided in three sections because the search for the physical page entry consists in a tree structure, where each of the 9 bit portion is used in a different page table to find that specific physical address. This three-level structure allows a more efficent way of recording Page Table entries (nearby addresses will have an equal VPN[2] and VPN[1] for example) and uses less memory, because instead of having a big singular portion of memory allocated, only the smaller section are allocated for what is needed at that moment. [PTE images] Each Page table Entry contains flag bits.
The map_page function maps a virtual page to a physical one in the three-level page table. Given a virtual and physical address creates the chain of page tables and inserts the final entry (with the correct flags). It works by breaking down `vaddr` in three VPNs indexes, each one is the PPN of a specific table. It reads a level at a time: if that level is not valid then it allocates a new page for the following table, if the level is valid then it extracts the PPN and it converts it in the physical address of the next table.
Because when a trap or the system enter S-mode I need to be able to read from the kernel every process saves a page table that points to the kernel itself, because `satp` gets overwritten with the current process page table address.

`Applications`
To implement user applications I first need a new linkre script to tell the compiler where to put it in memory. It resembles the kernel one but with a fundamental change: a different base address to not overlap kernel's address space.


`Sistem Calls`
There are many function written in this code at this point that can only work in S-mode (and in the kernel space). To use said functions in User Mode I need to implements the system calls: these are functions that once called give up the control od the execution to the kernel, "asking" it to execute a certain operation. In order to do so I had to modify my printc function, that will now receive as a parameter a single character, it will push that single character to the buffer and then it will call the UART module. I had to implement such modification because now I want the user space to be able to use the printf function, but has it was before I was not possible because it was using directly the buffer, modules such as UART, and other things limited to the kernel. Now the printf function can work in user space because all the operation in the kernel space are done by calling a printc function, that is the syscall to the printc function in the kernel if the printf is called in the user space. I also changed the distribution of the functions between kfunctions and common so I can include only common in user to achieve a greater security. 

To implement a getchar function I need to use the UART module in read mode. To do so, reading from the ti.com documentation, I need to set the value of the DLAB bit in the LCR register (add photo) to 0 and then use the shared read and write register THR/RBR. In order to check wheter there is something no the receiver line I have to check the IIR register. Then I can read the register and save the calue in a read buffer and echo what I just read to the user on the console. Echo is now in the user space so I can manipulate it. THe getc function in the user space works by calling the syscall to receive what was read: if nothing was read a sys_yield is called, otherwise the character gets echoed and added to the commandline buffer. To make the yield work with user processes I need to add to the trap s and trap_frame struct the user sp, so it can be later retrieved at the end of trap.s. I also implemented an exit syscall that mark the process as exited and no more runnable and the possibility to use backspace to delete characters.

## How to debug

Qemu launched in debug mode (command `make debug`, provided in the Makefile) starts a GDB server and stops at the first instruction, waiting for input from the debugger. To connect to the debugger, QEMU will open a port on the network: we will connect to that with another terminal (in the same folder) with this command: `riscv64-elf-gdb kernel.elf`. To connect to the QEMU port we will use `target remote localhost:1234`.

Once in GDB and connected to QEMU, these commands can be used:

- `layout split` — to see the C code, the disassembled code in Assembly, and the command line
- `b function` — to tell GDB to stop once `function` is called (`b` is for breakpoint)
- `c` — to tell the CPU to continue until the next breakpoint
- `si` — to tell the CPU to execute the next hardware instruction (this command also works to debug assembly code)
- `n` — to execute the next C line (if there is a function call, the function gets executed and the debugger stops at the next line)
- `s` — like `n`, but if there is a function call it gets executed line by line
- `p variable` — to print to the console the current value of the variable
- `p/x $register_name` — to print to the console the current value of the register (`/x` to print in hexadecimal)
- `x/10c variable` — to print to the console the first n (in this case 10) bytes decoded as characters at the address of memory saved in the variable

## Roadmap

Free resources when a process is exited

- [ ] **2. Console input** — the step that turns the shell into a real shell:
  - [ ] Enable RX interrupts (`IER |= 1` in `UART_init`)
  - [ ] In the trap handler (`ir == 10` branch), when `LSR & 1` read RBR and push
        into a second ring buffer (separate from the TX one)
  - [ ] `SYS_GETCHAR` (returns `-1` if the buffer is empty) + `getchar()` in user space
  - [ ] Shell main loop: prompt, echo, line parsing
        (note: QEMU sends `\r` for Enter, not `\n`)

- [ ] **3. `SYS_EXIT` + process reclaim** — mark the exiting process `PROC_UNUSED`
        and switch away from the syscall handler; later, add `free_pages` so the
        allocator can reuse the pages of dead processes.

- [ ] **4. `SYS_PRINT` (write a whole buffer)** — one `ecall` per string instead of
        one per character; introduces the problem of validating user pointers
        (the kernel must check that the range really belongs to user space
        before copying it).

- [ ] **5. Preemptive scheduling via timer** — the most significant change:
        requires saving `sstatus` (and eventually FP state) in the trap frame
        and deciding how to perform the context switch from inside a trap.

- [ ] **6. Extras** — `__free_ram_end__` read from the device tree at boot,
        W^X for user pages, `%b` in `printf`, shrink the PLIC mapping from
        4 MiB to the 3 pages actually used, idle process with a `wfi` loop
        as its `ra`, ...