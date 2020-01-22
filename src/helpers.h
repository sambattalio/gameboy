#ifndef HELPERS_H
#define HELPERS_H

#include <stdio.h>
#include <stdint.h>

#ifndef DEBUG
#define DEBUG 0
#endif 

#define debug_print(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

int is_half_carry_add(uint8_t a, uint8_t b);
int is_half_carry_sub(uint8_t a, uint8_t b);

#endif