ENTRY(start) /*to tell the linker script the starting point of the program: in this case 'start' is a global instruction in an assembly file. 
                In this case */
SECTIONS {
    . = 0x1000000;

    /* machine code */
    .text :{
        KEEP(*(.text.start)); /*Because there is not a ELF parser (that would read e_entry filleb by ENTRY(...) and jump at that address)
                            at this point, a binary file is generated that is allocated exactly at the address 0x1000000. 
                            So, to start the process the kernel does not read e_entry but it writes sepc = USER_BASE and executes sret.
                            This KEEP instructions is fundamental to make sure that the section with this name (that will contain the assembly instruction
                            start) will actually be positioned at the start of the file by the linker. 
                            (If I would ever activate a garbage collector without KEEP this section could also be 
                            eliminated because it is actually never accessed normally with call/jal instructions but only
                            via hardware writing in the sepc register)*/
        *(.text .text.*);
    }

    /* read-only data */
    .rodata : ALIGN(8) {
        *(.rodata .rodata.*);
    }

    /* data with initial values */
    .data : ALIGN(8) {
        *(.data .data.*);
    }

    /* data that should be zero-filled at startup */
    .bss : ALIGN(8) {
        *(.bss .bss.* .sbss .sbss.*);

        . = ALIGN(16);
        . += 64 * 1024; /* 64KB */
        __U_stack_top = .;

       ASSERT(. < 0x1800000, "too large executable");
       /* ASSERT is a function used to evaluate a boolean expression in the link phase and if false it stops the linking phase with an error.
       In this case the function is used to verify that the whole program i*/
    }
}