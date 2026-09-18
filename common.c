#include "common.h"


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

char* htoa(char* str, uint64_t num) {//hexadecimale to string converter
    char hex_digits[] = "0123456789ABCDEF";
    int j = 0;
    for (int i = 15; i >= 0; i--) {
        uint64_t nibble = (num >> (i * 4)) & 0xf;
        str[j++] = hex_digits[nibble];
    }

    str[j] = '\0';
    return str;
}

void printc(char ch);

void printf(const char *str, ...) { //... tells the compiler that after str there can be a variable number of parameters of every type
    va_list vargs; //it's pointer to the stack where the optional parameters are saved
    va_start(vargs, str); //to inizialize the pointer vargs to the end of str (after which there are the others parameters saved in contigous memory)
    
    while(*str) {
        if (*str == '%') {
            str++;
            switch(*str) {
                case('d'): {
                int n = va_arg(vargs, int);//to get the next value in the stack with the type specified and to move the pointer after this value
                static char s[21]; //max number of characters for a 64 bit number
                itoa(s, n);
                for (int i = 0; i<21 && s[i] != '\0'; i++)  printc(s[i]);
                break;}
                
                case('x'):{
                uint64_t x = va_arg(vargs, uint64_t); //64 bit hexadecimal number
                static char s[17]; //static needs to be sure that it doesn't change or get corrupted in the stack
                htoa(s, x);
                for (int i = 0; i<21 && s[i] != '\0'; i++) printc(s[i]);
                break;
                }
                
                //case('b'): //to be implemented
                //break;
                
                case('s'):{
                const char *s = va_arg(vargs, char *);
                while (*s != '\0') {
                    printc(*s);
                    s++;
                }
                break;
                }
                
                case('\0'):
                printc('%');
                goto end;
                break;
                
                default:
                printc(*str);
            }
        }else printc(*str);
        str++;
    }
    end: 
    va_end(vargs); //to clean memory
}