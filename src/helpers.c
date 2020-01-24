#include "helpers.h"

int is_three_half_carry_add(uint8_t a, uint8_t b, uint8_t c) {
    uint8_t before = (a & 0xF0) + (b & 0xF0) + (c & 0xF0); // upper 4 bits

    uint8_t after  = (a + b + c) & 0xF0;

    return before != after;
}

int is_half_carry_add(uint8_t a, uint8_t b) {
    uint8_t before = (a & 0xF0) + (b & 0xF0); // upper 4 bits

    uint8_t after  = (a + b) & 0xF0;

    return before != after;
}

int is_three_half_carry_sub(uint8_t a, uint8_t b, uint8_t c) {
    return (((a & 0xF0) - (b & 0xF0) - (c & 0xF)) & 0x0F) > 0;
}

int is_half_carry_sub(uint8_t a, uint8_t b) {
    return (((a & 0xF0) - (b & 0xF0)) & 0x0F) > 0;
}


uint16_t get_16bit_value(uint8_t upper, uint8_t lower) {
    /*
     * generate the 16bit value from the values in two registers from upper and lower
     * if upper is FF and lower is 00, return value would be 0xFF00
     */

    return (upper << 8) | lower;
}

uint8_t get_upper_8bit_value(uint16_t value) {
    /* returns the highest 8 bits from the 16 bit value */
    return (value & 0xFF00) >> 8;
}

uint8_t get_lower_8bit_value(uint16_t value) {
    /* returns the lowest 8 bits from a 16 bit value */
    return value & 0xFF;
}
