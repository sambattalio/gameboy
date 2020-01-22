#ifndef HELPERS_H
#define HELPERS_H

#include <stdio.h>

#define DEBUG 1
#define debug_print(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

#define is_half_carry(a, b) ((((a&0xf) + (b&0xf))&0x10) == 0x10)

#endif