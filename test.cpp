#include <iostream>

int main() {
    __uint128_t n = 90000000.0;
    printf("Size: %ld, Value: %lld\n", sizeof(n), n);
    
    return 0;
}