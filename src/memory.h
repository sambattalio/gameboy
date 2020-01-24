#ifndef MEMORY_H
#define MEMORY_H

#include "proc.h"
void proc_initialize_memory(Proc * p);

uint8_t read_byte(Proc * p, uint16_t address);
void write_byte(Proc * p, uint16_t address, uint8_t value);

uint16_t read_word(Proc * p, uint16_t address);
void write_word(Proc * p, uint16_t address, uint16_t word);

void write_tile(Proc * p, uint16_t address, uint8_t value);

#endif