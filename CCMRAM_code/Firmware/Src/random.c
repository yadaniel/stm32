#include "random.h"

uint32_t random_next;

uint32_t rand(void) {
    random_next = random_next * 1103515243 + 12345;
    return (uint32_t)(random_next / 65536) % 32768;
}

void srand(uint32_t seed) {
    random_next = seed;
}
