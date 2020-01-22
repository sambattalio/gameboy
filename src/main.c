// main.c

#include <stdio.h>
#include "proc.h"
#include "cart.h"

int main(int argc, char **argv) {

    Proc* processor = proc_create();

    Cart* cartridge = cart_create("../roms/Dr. Mario (World).gb");

    cart_load(cartridge, processor);

    proc_read_word(processor);

    return 0; 
}