#ifndef kernel_h
#define kernel_h

//PANIC is defined as a macro and not a function because with a function __FILE__ and __LINE__ would be the ones 
//of the file where the function is defined, while a macro is replaced by the compiler where is called
//##__VA_ARGS__ is used to define macros that accept a variable number of arguments. ## removes the comma when there are no
//additional arguments
#define PANIC(fmt, ...)                                                        \
    do {                                                                       \
        printf("PANIC: %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);  \
        while (1) {}                                                       \
    } while (0) // do-while is commonly used to define macros because it cannot produce errors or uninteded behaviours when combined with statements like if
    

#endif

