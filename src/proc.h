#ifndef PROC_H
#define PROC_H

#include <stdint.h>

#define MEM_SIZE (1 << 13)

// excluding the flags register
typedef struct {
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;
} Registers;

typedef struct {
    int zero;
    int subtract;
    int half_carry;
    int carry;
} FlagRegister;

typedef struct {
    uint8_t memory[MEM_SIZE];

    unsigned pc;
    Registers registers;
    FlagRegister flagRegister;
} Proc;

Proc*          proc_create();
void           proc_delete(Proc* p);
void           proc_handle_word(Proc *p);
void           proc_initialize_memory(Proc * p);

#endif