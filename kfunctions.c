#include "kfunctions.h"
#include "UART.h"
#include "kernel.h"

#define BUFFER_SIZE 256

static volatile uint8_t buffer[BUFFER_SIZE]; //a volatile variable could change at any time and no action has to be taken
static volatile uint8_t head = 0; //because it's uint8_t when the value is 255 the next one will automatically be 0, so no need to implements the module operation
static volatile uint8_t tail = 0; 

static volatile uint8_t rbx[BUFFER_SIZE];
static volatile uint8_t rbx_h = 0;
static volatile uint8_t rbx_t = 0;

int buffer_is_empty(){
    return head == tail;
}

int buffer_is_full() {
    return (uint8_t)(head + 1) == tail;
}

void buffer_add(char c) {
    if (buffer_is_full()) return;
    buffer[head++] = c;
}

uint8_t buffer_next(){
    if (buffer_is_empty()) return 0;
    return buffer[tail++];
}

int rbx_is_empty(){
    return rbx_h == rbx_t;
}

int rbx_is_full() {
    return (uint8_t)(rbx_h + 1) == rbx_t;
}

void rbx_add(char ch) {
    rbx[rbx_h++] = ch;
}

int rbx_next(){
    if (rbx_is_empty()) return -1;
    return rbx[rbx_t++];
}

paddr_t alloc_pages(uint64_t n) { //this function allocates n pages of memory and returns the starting address
    static paddr_t next_paddr = (paddr_t) __free_ram__;
    paddr_t paddr = next_paddr;

    if (next_paddr + n * PAGE_SIZE > (paddr_t) __ram_end__)
    PANIC("alloc_pages: out of memory");
    
    next_paddr += n * PAGE_SIZE;

    memset((void *) paddr, 0, n * PAGE_SIZE); //the function has to return a clean portion of memory set at zero, because map_page uses a bit of this area of memory in order to declare if the entry is valid or not inizialized
    return paddr;
}

void panic_printf(const char *str, ...) {
    va_list vargs;
    UART_reset_o(); //to stop the interrupt when a new character is sent to the UART
    va_start(vargs, str);

    while(*str) {
        if (*str == '%') {
            str++;
            switch(*str) {
                case('d'): {
                int n = va_arg(vargs, int);//to get the next value in the stack with the type specified and to move the pointer after this value
                static char s[21]; //max number of characters for a 64 bit number
                itoa(s, n);
                for (int i = 0; i<21 && s[i] != '\0'; i++) buffer_add(s[i]);
                break;}
                
                case('x'):{
                uint64_t x = va_arg(vargs, uint64_t); //64 bit hexadecimal number
                static char s[17]; //static needs to be sure that it doesn't change or get corrupted in the stack
                htoa(s, x);
                for (int i = 0; i<21 && s[i] != '\0'; i++) buffer_add(s[i]);
                break;
                }
                
                //case('b'): //to be implemented
                //break;
                
                case('s'):{
                const char *s = va_arg(vargs, char *);
                while (*s != '\0') {
                    buffer_add(*s);
                    s++;
                }
                break;
                }
                
                case('\0'):
                buffer_add('%');
                goto end;
                break;
                
                default:
                buffer_add(*str);
            }
        }else buffer_add(*str);
        str++;
    }
    end: 
    va_end(vargs); //to clean memory
    while(!buffer_is_empty()) {
        while(!UART_wReady()); //to wait until the other character is sent
        UART->THR_RBR = buffer_next(); 
    }
    return;
}

void set_sepc(uint64_t val) {
    asm volatile("csrw sepc, %0" :: "r"(val));
}

void printc(char ch){//print to console function
    buffer_add(ch);

    if (!UART_o_enabled()) { //to kick start only if the writing is not already started
        while (!UART_wReady());
        UART->THR_RBR = buffer_next();
        UART_enable_o();
    }
    return;
}

int getc() {
    return rbx_next();
}

