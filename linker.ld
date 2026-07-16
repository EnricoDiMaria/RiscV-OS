OUTPUT_ARCH( "riscv" )
ENTRY(__start__) /*first line of assembly code executed */

INCLUDE memory_map.ld

SECTIONS {
    . = 0x80000000; /*location counter: everything from this point on will be put */
    /*after this address*/

    /*text section where the code will go*/
    .text : {
        *(.init) /*to force the bootloader section at the very start of ram that is what a RISC-V system will execute at boot*/
        *(.text .text.*) /*it takes every .text section and put it in the ram address*/
        /*declared in memory_map*/
    } > ram 

    /*read-only data section*/
    .rodata : {
    . = ALIGN(8); /*every 8 bytes for RISC-V 64 bit*/
    *(.rodata .rodata.*)
  } > ram

  /*data section*/
  .data : {
    . = ALIGN(16);
    __global_pointer$ = . + 0x800; /*0x800 is 2048, the maximum number for a 12 bit immediate (offset)*/
    /*the gp label is stored in the gp register that saves the address of .data+2048 (center of the section), so the compiler can use this to access different position in data with a positive or negative offset more efficiently. For longer distances more than one instruction (to load an immediate larger than 12 bits) will be needed.*/
    *(.data .data.*)
  } > ram

  .bss : {
        . = ALIGN(16);
        __bss_start__ = .; /*the linker puts a label at this address*/
        *(.bss .bss.*)
        __bss_end__ = .;
    } > ram

    . = ALIGN(16);
    . += 128 * 1024; /* 128KB */
    __stack_top__ = .; /*to put the start of the stack after 128KB the last bss (stack dimension = 128 KB)*/
}


/*to be implemented
.sdata
.sbss
these are smaller section used to store smaller variables (like pointers, flags or contants) that are totally addressable with a 12 bit immediate (thus more efficiently).
*/
