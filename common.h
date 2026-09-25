#ifndef common_h
#define common_h

typedef unsigned char uint8_t; 
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef uint64_t size_t;
typedef uint64_t paddr_t; //physical memory address
typedef uint64_t vaddr_t; //virtual memory address

#define true  1
#define false 0
#define NULL  ((void *) 0)
#define PAGE_SIZE 4096

#define va_list  __builtin_va_list //to not rely on standard libraries we use compiler builtins
#define va_start __builtin_va_start
#define va_end   __builtin_va_end
#define va_arg   __builtin_va_arg

#define align_up(value, align)   (((value) + (align) - 1) & ~((align) - 1)) //to round up an address to make it a perfect multiple of the align value
#define is_aligned(value, align) (((value) & ((align) - 1)) == 0) // returns true or false if the value is aligned with align
#define offsetof(type, member)   __builtin_offsetof(type, member) //it returns the distance from the start of the struct and the start of the member specified
//used because assembly cannot comprehend structs so it needs the offset of every member

#define SYS_PRINTC 1
#define SYS_GETC 2
#define SYS_EXIT 3
#define SYS_YIELD 4
#define SYS_SLEEP 5

void *memset(void *target, char c, size_t n);
void *memcpy(void *dst, const void *src, size_t n);
char *strcpy(char *dst, const char *src);
char * strcpy_s(char *dst, size_t dest_size, const char *src);
int strcmp(const char *s1, const char *s2);
char *itoa(char *str, int num);
char *htoa(char *str, uint64_t num);

void printf(const char *str, ...);

#endif