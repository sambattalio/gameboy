#include "helpers.h"

int is_half_carry_add(uint8_t a, uint8_t b) {
    uint8_t before = (a & 0xF0) + (b & 0xF0); // upper 4 bits

    uint8_t after  = (a + b) & 0xF0;

    return before != after;
}

int is_half_carry_sub(uint8_t a, uint8_t b) {
    uint8_t test = (a & 0xF) - (b & 0xF); // sub lower 4 bits

    return test > 0xF;
}