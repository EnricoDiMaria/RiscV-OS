unsigned long long power2exp(unsigned long long number) {
    if (number <=1) return 0;
    number--; //this function returns the next power of 2 so if the number is already a power of two without decrementing it would not consider the number itself
    unsigned long long zeros = __builtin_clzll(number);
    /* __builtin_clzll(n) counts the leading zeros from the left in a 64-bit integer. Since the leftmost 1 bit 
    represents the floor power of 2 (2^k<= n), subtracting this count from 64 directly yields the ceiling exponent
    (k+1) such that (2^x >= number) in a single CPU clock cycle.*/
    return 64 - zeros;
}