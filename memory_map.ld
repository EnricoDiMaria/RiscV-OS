MEMORY {
    ram (wxa) : ORIGIN = 0x80000000, LENGTH = 128M
    /*in virt ram starts at 0x80000000. The attributes for this section of memory*/
    /*are: w = writable, x = executable, a = allocable*/
}

/*variables to use in the C kernel*/
__ram_start__ = ORIGIN(ram);
__ram_size__  = LENGTH(ram);
__ram_end__   = ORIGIN(ram) + LENGTH(ram);

