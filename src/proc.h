#ifndef PROC_H
#define PROC_H

#include <stdint.h>
#include <stdlib.h>

#include "helpers.h"

#define MEM_SIZE (1 << 16)

// excluding the flags register
typedef struct {
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t f;
    uint8_t h;
    uint8_t l;
} Registers;

enum FlagState {
    SET   = 1,
    CLEAR = 0
};

typedef struct {
    int zero;
    int subtract;
    int half_carry;
    int carry;
} FlagRegister;

typedef struct {
    uint8_t memory[MEM_SIZE];

    Registers registers;
    FlagRegister flagRegister;
    
    uint16_t pc;
    uint16_t sp;
} Proc;

Proc*          proc_create();
void           proc_delete(Proc* p);
void           proc_read_word(Proc* p);
void           proc_initialize_memory(Proc* p);

void           proc_dec_hl(Proc* p);
void           proc_inc_hl(Proc* p);

#endif