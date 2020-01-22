#include "cart.h"

Cart* cart_create(char* rom_file) {
    Cart* c = calloc(1, sizeof(Cart));

    if (c) {
        memset(c->rom, 0, CART_SIZE);
        cart_read(c, rom_file);
    }

    return c;
}

/* Helper function to read from file into the unsigned char arr */
void cart_read(Cart* c, char *rom_file) {
    if (!c) return;

    FILE *f = fopen(rom_file, "rb");
    if (!f) {
        printf("Error opening file '%s'!\n", rom_file);
        return;
    }

    // Find file size
    fseek(f, 0L, SEEK_END);
    int size = ftell(f);
    // check if size too big
    if (size > CART_SIZE) {
        printf("Error: Rom too big!\n");
        fclose(f);
        return;
    }
    // Reset pointer
    fseek(f, 0L, SEEK_SET);

    fread(c->rom, size, 1, f);

    fclose(f);
}


/* Add memory in from from 0x0000 -> 0x8000 to processor */
void cart_load(Cart* c, Proc* p) {
    if (!c || !p) return;

    for (uint32_t i = 0; i < CART_SIZE; i++) {
        p->memory[i] = c->rom[i];
    }
}

void cart_delete(Cart* c) {
    if (!c) return;
    free(c);
}