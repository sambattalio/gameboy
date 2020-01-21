// proc.c
#include "proc.h"


proc* proc_create() {
    proc *p = calloc(1, sizeof(proc));
    if (p) {
        p->pc = 0x100;
    }
    return p;
}

void proc_delete(proc* p) {
    if (!p) return;
    free(p);
}

// follows instruction set defined @ https://gekkio.fi/files/gb-docs/gbctr.pdf
void proc_handle_word(proc *p) {
    if (!p) return;

    unsigned char* word = &p->memory[p->pc];

    uint8_t eightbit_opcode = p->memory[p->pc];

    // https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html
    switch (eightbit_opcode) {
        case 0x00: // NOP
            // 1 byte instruction total 
            break;
        case 0x01: // LD BC, d16 (use BC as 16 bit register)
            // 3 byte instruction total
            break;
        case 0x02: // LD (BC), A (load A into BC)
            // 1 byte inst
            break;
        case 0x03: // INC BC
            // 1 byte inst
            break;
        case 0x04: // INC B
            // 1 byte inst
            // Z 0 H - flags
            break;
        case 0x05: // DEC B
            // 1 byte inst
            // Z 1 H - flags
            break;
        case 0x06: // LD B, d8
            // 2 byte inst
            break;
        case 0x07: // RLCA
            // 1 byte
            // 0 0 0 C
            break;
        case 0x08: // LD (a16), SP
            // 3 byte
            break;
        case 0x09: // ADD HL, BC
            // 1
            // - 0 H C
            break;
        case 0x0A: // LD A, (BC)
            // 1
            break;
        case 0x0B: // DEC BC
            // 1
            break;
        case 0x0C: // INC C
            // 1
            // Z 0 H -
            break;
        case 0x0D: // DEC C
            // 1
            // Z 1 H -
            break;
        case 0x0E: // LD C, d8
            // 2 byte
            break;
        case 0x0F: //RRCA
            // 1
            // 0 0 0 C
            break;
        case 0x10: // STOP 0
            // 2
            break;
        case 0x11: // LD DE, d16
            // 3
            break;
        case 0x12: // LD (DE), A
            // 1
            break;
        case 0x13: // INC DE
            // 1
            break;
        case 0x14: // INC D
            // 1
            // Z 0 H -
            break;
        case 0x15: // DEC D
            // 1
            // Z 1 H -
            break;
        case 0x16: // LD D, d8
            // 2
            break;
        case 0x17: // RLA
            // 1
            // 0 0 0 C
            break;
        case 0x18: // JR r8
            // 2
            break;
        case 0x19: // ADD HL, DE
            // 1
            // - 0 H C
            break;
        case 0x1A: // LD A, (DE)
            // 1
            break;
        case 0x1B: // DEC DE
            // 1
            break;
        case 0x1C: // Inc E
            // 1
            // Z 0 H -
            break;
        case 0x1D: // DEC E
            // 1
            // Z 1 H -
            break;
        case 0x1E: // LD E, d8
            // 2
            break;
        case 0x1F: // RRA
            // 1
            // 0 0 0 C
            break;
    }
}
