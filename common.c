#include "common.h"
#include "UART.h"

#define BUFFER_SIZE 256

static volatile uint8_t buffer[BUFFER_SIZE]; //a volatile variable could change at any time and no action has to be taken
static volatile uint8_t head = 0; //because it's uint8_t when the value is 255 the next one will automatically be 0, so no need to implements the module operation
static volatile uint8_t tail = 0; 

int buffer_full(void) {return ((uint8_t)(head+1) == tail);}
int uart_done(void) {return (head==tail);}

uint8_t buffer_next(){
    return buffer[tail++];
}

void *memset(void *target, char c, size_t n) {
    uint8_t *p = (uint8_t *) target; //with a char caster we force the CPU to work in bytes 
                                    //(with a void pointer it would not know how big the single data would be)
    while (n--) {
        *p++ = c;
    }
    return target;
}

void *memcpy(void *dst, const void *src, size_t n) {
    uint8_t *d = (uint8_t *) dst;
    const uint8_t *s = (const uint8_t *) src;
    while (n--)
        *d++ = *s++;
    return dst;
}

char * strcpy(char *dst, const char *src) {//this function copies until src ends, so even if the area of memory allocated for dst is shorter (not very secure!)
    char *d = dst;
    while(*src) *d++ = *src++;
    *d = '\0';
    return dst;
}

char * strcpy_s(char *dst, size_t dest_size, const char *src) { //dest_size is in byte, so it's the number of characters   
    if (dst == NULL || src == NULL || dest_size == 0) {
        if (dst != NULL && dest_size > 0) *dst = '\0';
        return NULL;
    }

    size_t i = 0;
    char *d = dst;
    while(*src) {
        if (i>=dest_size-1) {
            *dst = '\0';
            return NULL;}
        
        *d++ = *src++;
        i++;
        }
    *d = '\0';
    return dst;
}

int strcmp(const char *s1, const char *s2) {
    while(*s1 && *s2) {
        if (*s1!=*s2) break;
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

static void buffer_write(const char *string) { //const is used for a read-only variable
    while(*string != '\0') {
        buffer[head++] = (uint8_t) *string;
        string++;
    }
}

void printc(const char *string) { //print to console function
    int uart_was_sleeping = uart_done(); //I need to check before writing in buffer becauase buffer_write increments head so it would be always walse otherwhise
    buffer_write(string);
    if (uart_was_sleeping) UART->THR = buffer[tail++];
    //this function send the first characters, the others will be sent by the exception_handler
    //only if the uart is not already in use
    //because when written THR sends an interrupt to get the others characters
    return;
}


void buffer_add(char c) {
    buffer[head++] = c;
    return;
}

char * itoa(char *str, int num) { //standard itoa function
    int i = 0;
    int is_negative = 0;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;}

    if (num < 0) {
        is_negative = 1;
        num = -num;}

    while (num != 0) {//numbers are added in the opposite order
        int rem = num % 10;
        str[i++] = rem + '0';
        num = num / 10;
    }
    if (is_negative) str[i++] = '-';
    str[i] = '\0';

    //to reverse the string
    i--;
    for (int j = 0; j<i;) {
        char temp = str[j];
        str[j] = str[i];
        str[i] = temp;
        j++;
        i--;
    }
    return str;
}

char* htoa(char* str, unsigned int num) {//hexadecimale to string converter
    char hex_digits[] = "0123456789ABCDEF";
    int j = 0;
    for (int i = 7, j=0; i >= 0; i--) {
        unsigned nibble = (num >> (i * 4)) & 0xf;
        str[j++] = hex_digits[nibble];
    }

    str[j] = '\0';
    return str;
}

void printf(const char *str, ...) { //... tells the compiler that after str there can be a variable number of parameters of every type
    va_list vargs; //it's pointer to the stack where the optional parameters are saved
    va_start(vargs, str); //to inizialize the pointer vargs to the end of str (after which there are the others parameters saved in contigous memory)

    int uart_was_sleeping = uart_done(); //I need to check before writing in buffer becauase buffer_write increments head so it would be always walse otherwhise
    while(*str) {
        if (*str == '%') {
            str++;
            switch(*str) {
                case('d'): {
                int n = va_arg(vargs, int);//to get the next value in the stack with the type specified and to move the pointer after this value
                static char s[21]; //max number of characters for a 64 bit number
                buffer_write(itoa(s, n));
                break;}
                
                case('x'):{
                unsigned x = va_arg(vargs, unsigned); //32 bit hexadecimal number = max 8 characters
                static char s[9]; //static needs to be sure that it doesn't change or get corrupted in the stack
                buffer_write(htoa(s, x));
                break;
                }
                
                //case('b'): //to be implemented
                //break;
                
                case('s'):{
                const char *s = va_arg(vargs, char *);
                buffer_write(s);
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
    if (uart_was_sleeping) UART->THR = buffer[tail++];
    return;
}