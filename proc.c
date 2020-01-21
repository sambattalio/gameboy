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
        case 0x20: // JR NZ, r8
            // 2
            break;
        case 0x21: // LD HL, d16
            //3 
            break;
        case 0x22: // LD (HL+), A
            // 1
            break;
        case 0x23: // INC HL
            // 1
            break;
        case 0x24: // INC H
            // 1
            // Z 0 H -
            break;
        case 0x25: // DEC H
            // 1
            // Z 1 H -
            break;
        case 0x26: // LD H, d8
            // 2
            break;
        case 0x27: // DAA
            // 1
            // Z - 0 C
            break;
        case 0x28: // JR Z,r8
            // 2
            break;
        case 0x29: // ADD HL, HL
            // 1
            // - 0 H C
            break;
        case 0x2A: // LD A, (HL+)
            // 1
            break;
        case 0x2B: // DEC HL
            // 1
            break;
        case 0x2C: //INC L
            // 1
            // Z 0 H -
            break;
        case 0x2D: // DEC L
            // 1
            // Z 1 H -
            break;
        case 0x2E: // LD L, d8
            // 2
            break;
        case 0x2F: // CPL
            //1
            // - 1 1 -
            break;
        case 0x30: // JR NC,r8
            // 2
            break;
        case 0x31: // LD SP, d16
            // 3
            break;
        case 0x32: // LD (HL-), A
            //1
            break;
        case 0x33: // INC SP
            // 1
            break;
        case 0x34: // INC (HL)
            // 1
            // Z 0 H -
            break;
        case 0x35: // DEC (HL)
            // 1
            // Z 1 H -
            break;
        case 0x36: // LD (HL), d8
            // 2
            break;
        case 0x37: // SCF
            // 1
            // - 0 0 1
            break;
        case 0x38: // JR C, r8
            // 2
            break;
        case 0x39: // ADD HL, SP
            // 1
            // - 0 H C
            break;
        case 0x3A: // LD A, (HL-)
            // 1
            break;
        case 0x3B: // DEC SP
            // 1
            break;
        case 0x3C: // INC A
            // 1
            // Z 0 H -
            break;
        case 0x3D: // DEC A
            // 1
            // Z 1 H -
            break;
        case 0x3E: // LD A, d8
            // 2
            break;
        case 0x3F: // CCF
            // 1
            // - 0 0 C 
            break;
        case 0x40: // LD B, B
            // 1
            break;
        case 0x41: // LD B, C
            // 1
            break;
        case 0x42: // LD B, D
            // 1
            break;
        case 0x43: // LD B, E
            // 1
            break;
        case 0x44: // LD B, H
            // 1
            break;
        case 0x45: // LD B, L
            // 1
            break;
        case 0x46: // LD B, (HL)
            // 1
            break;
        case 0x47: // LD B, A
            // 1
            break;
        case 0x48: //LD C, B
            // 1
            break;
        case 0x49: //LD C, C
            // 1
            break;
        case 0x4A: //LD C, D
            // 1
            break;
        case 0x4B: //LD C, E
            // 1
            break;
        case 0x4C: //LD C, H
            // 1
            break;
        case 0x4D: //LD C, L
            // 1
            break;
        case 0x4E: // LD C, (HL)
            // 1
            break;
        case 0x4F: // LD C, A
            // 1
            break;
        case 0x50: // LD D, B
            // 1
            break;
    }
}
