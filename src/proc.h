#ifndef PROC_H
#define PROC_H

#include <stdint.h>
#include <stdlib.h>

#include "helpers.h"

#define MEM_SIZE (1 << 13)

typedef struct {
    uint8_t memory[MEM_SIZE];

    uint16_t pc;
    uint16_t sp;
} proc;

proc*          proc_create();
void           proc_delete(proc* p);
void           proc_read_word(proc *p);
void           proc_initialize_memory(proc * p);

#endif