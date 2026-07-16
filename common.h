#ifndef common_h
#define common_h


typedef unsigned char uint8_t; 
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef uint64_t size_t;

#define true  1
#define false 0
#define NULL  ((void *) 0)

//defined using char to work in bytes and have the location of the single byte
extern uint8_t __ram_start__;
extern uint8_t __ram_size__;
extern uint8_t __ram_end__;
extern uint8_t __stack_top__;
extern uint8_t __bss_start__; //"the value at the 0th byte of the .bss section"
extern uint8_t __bss_end__;

//__builtin_ functions are Clang-specific extensions (https://clang.llvm.org/docs/LanguageExtensions.html)
#define va_list  __builtin_va_list //to not rely on standard libraries we use compiler builtins
#define va_start __builtin_va_start
#define va_end   __builtin_va_end
#define va_arg   __builtin_va_arg

#define align_up(value, align)   __builtin_align_up(value, align) //to round up an address to make it a perfect multiple of the align value
#define is_aligned(value, align) __builtin_is_aligned(value, align) // returns true or false if the value is aligned with align
#define offsetof(type, member)   __builtin_offsetof(type, member) //it returns the distance from the start of the struct and the start of the member specified
//used because assembly cannot comprehend structs so it needs the offset of every member

int buffer_full();
int uart_done();
uint8_t buffer_next();

void *memset(void *target, char c, size_t n);
void *memcpy(void *dst, const void *src, size_t n);
char *strcpy(char *dst, const char *src);
char * strcpy_s(char *dst, size_t dest_size, const char *src);
int strcmp(const char *s1, const char *s2);
void printc(const char *string);
void printf(const char *str, ...);




#endif