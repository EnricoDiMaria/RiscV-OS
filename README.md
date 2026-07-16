# WRITING A BOOTLOADER/KERNEL/OS in RISC-V 64 bit FROM SCRATCH

## Memory Mapping and Linker

* First of all, we need to write the `memory_map.ld` to tell the compiler where to put the start of the `.text` code from the bootloader (`boot.s`).


* The `memory_map` will be later included in the `linker.ld`.


* Since we want to run the OS on `qemu-virt`, from QEMU documentation we can see that the virt RAM starts at address `0x80000000`.


* The virt machine uses a large RAM memory to store everything (bootloader, kernel, data), hence we just declare this memory section and not separate ones.


* In `linker.ld` we declare sections to decide where and in what order to put the compiled program in memory.



## Bootloader and Kernel Initialization

* Now it’s time to start writing the bootloader itself, which I decided to write directly in assembly.


* First of all, I need to access the variables declared in the linker.


* Then I write the procedure called `__start__` (same name as in the linker), where I save the stack pointer (needed to execute any C function), I load `mtvec` with an assembly exception handler address, and I call the kernel in M-mode for now.


* In the kernel, the first function implemented is `memset` (to set a specific section of memory to a specified value).


* The function is used right after calling the kernel to initialize the `.bss` section to zero.



## Compilation and Execution

* A `Makefile` is provided to compile and run the OS with a single line in the terminal (`make run` to run, `make clean` to clean).


* To access the QEMU console, the shortcut is **Ctrl+A** followed by **C** (console).


* To get info about registers, the `info registers` command can be used: from this section, it can be easily seen that the PC is correctly set to a value that is the offset from the RAM start address (`0x80000000`) and a line of the `kernel_main` function address at the point of execution.


* This can be easily verified by disassembling the executable with `riscv64-elf-objdump -d kernel.elf | less`.



## Console Output/Input and UART

* To implement a console output/input we will use UART (Universal Asynchronous Receiver / Transmitter), emulated by QEMU, that works by writing at the `0x10000000` address (it’s Memory Mapped I/O).


* When we write something to this address (or an offset of this address) we are actually communicating with the UART device.


* Reading from a pointer pointing to that address extracts 8 bits from the receiver (RBR), while writing to said pointer transmits 8 bits to the transmitter (THR).


* Even though QEMU is virtualized so the output from the UART register to the console is almost immediate, I decided to implement a check on the Line Status Register (LSR), that is how real hardware would tell us if the character has been sent.



## Printing and Interrupts

* I want to implement a function (`printc`) to print a string.


* To do so I need to implement first a Ring Buffer (FIFO), from which the interrupt handler will read every character in order.


* We implement the FIFO with a circular buffer with head (to write) and tail (to read) pointers.


* The exception handler is now empty: the first thing to implement is saving all registers and passing `mcause`, `mtval`, and `mepc` as parameters to the `trap_handler` C function.


* The PLIC and UART modules are also implemented (further explanations on how they work are in the respective `.c` and `.h` files).


* At the start of the kernel, `PLIC_enable_interrupts_modules` is called to enable and set priority of the modules specified in that function.


* The `printc` function works by saving the string in the buffer, then it checks if the UART was already writing something.


* If it was, then it will automatically get to the end of the buffer and print the new string; if it wasn’t, then `printc` puts the first character in the receiver register of the UART.


* When a new character is written in THR, then it is sent in output and then the UART module sends an interrupt.


* I use this interrupt to finish writing the rest of the buffer: the exception handler, when called by said interrupt, understands that it was the UART calling.


* If the UART is Ready (it sent the interrupts so it is already ready, the control on LSR is just to be sure) and the buffer has not been already cleared, then it sends the next character to the THR and the loop goes on like that until the end of the buffer, when UART interrupts are disabled.


* The `printc` function and the buffer are now moved to a `common.c`/`common.h` file in order to make them accessible from every program, not only the kernel.


* In this file I also include the structures, and I need to do so also to implement a `printf` function (that can support format specifiers and variables).


* My `printf` implementation works with a switch case, when a `%` is read, that gets the correct value and the string is progressively written in the buffer.


* Only at the end the first character will be sent to the UART that will start outputting the string.


* Next, I implement some standard C functions that will be useful in the OS.



## Debugging with GDB

* QEMU launched in debug mode (command `make debug` provided in the Makefile) starts a GDB server and stops at the first instruction waiting for an input from the debugger.


* To connect to the debugger, QEMU will open a port on the network: we will connect to that with another terminal (in the same folder) with this command: `riscv64-elf-gdb kernel.elf`.


* To connect to the QEMU port we will use `target remote localhost:1234`.


* Once in GDB and connected to QEMU these commands can be used:


* `layout split` to see the C code, the disassembled code in Assembly, and the command line.


* `b function` to tell GDB to stop once function is called (`b` is for break).


* `c` to tell the CPU to continue until the next breakpoint.


* `si` to tell the CPU to execute the next hardware instruction (this command works also to debug assembly code).


* `n` to execute the next C line (if there is a function call the function gets executed and the debugger stops at the next line).


* `s` like next but if there is a function call it gets executed line by line.


* `p variable` to print to the console the current value of the variable.


* `p/x $register_name` to print to the console the current value of the register (`/x` to print in hexadecimal).


* `x/10c variable` to print to the console the first `n` (in this case 10) bytes decoded as characters at the address of memory saved in the variable.
