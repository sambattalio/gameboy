#ifndef HELPERS_H
#define HELPERS_H

#include <stdio.h>

#define DEBUG 1
#define debug_print(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

#define is_half_carry_add(a, b) ((a & 0xf) + (b & 0xf) > 0xf)
#define is_half_carry_sub(a, b) (((a & 0xf) - (b & 0xf)) < 0)

int is_half_carry_add(uint8_t a, uint8_t b);
int is_half_carry_sub(uint8_t a, uint8_t b);

#endif