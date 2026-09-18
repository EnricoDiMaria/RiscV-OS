#ifndef kernel_h
#define kernel_h

#include "kfunctions.h"

//PANIC is defined as a macro and not a function because with a function __FILE__ and __LINE__ would be the ones 
//of the file where the function is defined, while a macro is replaced by the compiler where is called
//##__VA_ARGS__ is used to define macros that accept a variable number of arguments. ## removes the comma when there are no
//additional arguments
#define PANIC(fmt, ...)                                                        \
    do {                                                                       \
        printf("PANIC: %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);  \
        while (1) {}                                                       \
    } while (0) // do-while is commonly used to define macros because it cannot produce errors or uninteded behaviours when combined with statements like if


struct trap_frame {
    uint64_t ra;
    uint64_t gp;
    uint64_t tp;
    uint64_t t0;
    uint64_t t1;
    uint64_t t2;
    uint64_t t3;
    uint64_t t4;
    uint64_t t5;
    uint64_t t6;
    uint64_t a0;
    uint64_t a1;
    uint64_t a2;
    uint64_t a3;
    uint64_t a4;
    uint64_t a5;
    uint64_t a6;
    uint64_t a7;
    uint64_t s0;
    uint64_t s1;
    uint64_t s2;
    uint64_t s3;
    uint64_t s4;
    uint64_t s5;
    uint64_t s6;
    uint64_t s7;
    uint64_t s8;
    uint64_t s9;
    uint64_t s10;
    uint64_t s11;
    uint64_t sp;
} __attribute__((packed));

#endif