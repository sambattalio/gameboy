#ifndef CART_H
#define CART_H

#include <string.h>
#include "helpers.h"
#include "proc.h"

#define CART_SIZE (1 << 15)

// DEAL WITH MEMORY BANKS LATER

typedef struct {
    unsigned char rom[CART_SIZE];
} Cart;

Cart* cart_create(char* rom_file);
void  cart_read(Cart* c, char* rom_file);
void  cart_load(Cart* c, Proc* p);
void  cart_delete(Cart* c);


#endif