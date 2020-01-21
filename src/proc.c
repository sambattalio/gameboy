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
        case 0x0:
            // NOP
            // 1  4
            // - - - -
            break;
        case 0x1:
            // LD BC
            // 3  12
            // - - - -
            break;
        case 0x2:
            // LD
            // 1  8
            // - - - -
            break;
        case 0x3:
            // INC BC
            // 1  8
            // - - - -
            break;
        case 0x4:
            // INC B
            // 1  4
            // Z 0 H -
            break;
        case 0x5:
            // DEC B
            // 1  4
            // Z 1 H -
            break;
        case 0x6:
            // LD B
            // 2  8
            // - - - -
            break;
        case 0x7:
            // RLCA
            // 1  40
            // 0 0 0 C
            break;
        case 0x8:
            // LD
            // 3  20
            // - - - -
            break;
        case 0x9:
            // ADD HL
            // 1  8
            // - 0 H C
            break;
        case 0xA:
            // LD A
            // 1  8
            // - - - -
            break;
        case 0xB:
            // DEC BC
            // 1  8
            // - - - -
            break;
        case 0xC:
            // INC C
            // 1  4
            // Z 0 H -
            break;
        case 0xD:
            // DEC C
            // 1  4
            // Z 1 H -
            break;
        case 0xE:
            // LD C
            // 2  8
            // - - - -
            break;
        case 0xF:
            // RRCA
            // 1  40
            // 0 0 0 C
            break;
        case 0x10:
            // STOP 02
            // 2  4
            // - - - -
            break;
        case 0x11:
            // LD DE
            // 3  12
            // - - - -
            break;
        case 0x12:
            // LD
            // 1  8
            // - - - -
            break;
        case 0x13:
            // INC DE
            // 1  8
            // - - - -
            break;
        case 0x14:
            // INC D
            // 1  4
            // Z 0 H -
            break;
        case 0x15:
            // DEC D
            // 1  4
            // Z 1 H -
            break;
        case 0x16:
            // LD D
            // 2  8
            // - - - -
            break;
        case 0x17:
            // RLA
            // 1  40
            // 0 0 0 C
            break;
        case 0x18:
            // JR r82
            // 2  12
            // - - - -
            break;
        case 0x19:
            // ADD HL
            // 1  8
            // - 0 H C
            break;
        case 0x1A:
            // LD A
            // 1  8
            // - - - -
            break;
        case 0x1B:
            // DEC DE
            // 1  8
            // - - - -
            break;
        case 0x1C:
            // INC E
            // 1  4
            // Z 0 H -
            break;
        case 0x1D:
            // DEC E
            // 1  4
            // Z 1 H -
            break;
        case 0x1E:
            // LD E
            // 2  8
            // - - - -
            break;
        case 0x1F:
            // RRA
            // 1  40
            // 0 0 0 C
            break;
        case 0x20:
            // JR NZ
            // 2  12
            // - - - -
            break;
        case 0x21:
            // LD HL
            // 3  12
            // - - - -
            break;
        case 0x22:
            // LD
            // 1  8
            // - - - -
            break;
        case 0x23:
            // INC HL
            // 1  8
            // - - - -
            break;
        case 0x24:
            // INC H
            // 1  4
            // Z 0 H -
            break;
        case 0x25:
            // DEC H
            // 1  4
            // Z 1 H -
            break;
        case 0x26:
            // LD H
            // 2  8
            // - - - -
            break;
        case 0x27:
            // DAA
            // 1  4
            // Z - 0 C
            break;
        case 0x28:
            // JR Z
            // 2  12
            // - - - -
            break;
        case 0x29:
            // ADD HL
            // 1  8
            // - 0 H C
            break;
        case 0x2A:
            // LD A
            // 1  8
            // - - - -
            break;
        case 0x2B:
            // DEC HL
            // 1  8
            // - - - -
            break;
        case 0x2C:
            // INC L
            // 1  4
            // Z 0 H -
            break;
        case 0x2D:
            // DEC L
            // 1  4
            // Z 1 H -
            break;
        case 0x2E:
            // LD L
            // 2  8
            // - - - -
            break;
        case 0x2F:
            // CPL
            // 1  4
            // - 1 1 -
            break;
        case 0x30:
            // JR NC
            // 2  12
            // - - - -
            break;
        case 0x31:
            // LD SP
            // 3  12
            // - - - -
            break;
        case 0x32:
            // LD
            // 1  8
            // - - - -
            break;
        case 0x33:
            // INC SP
            // 1  8
            // - - - -
            break;
        case 0x34:
            // INC
            // 1  12
            // Z 0 H -
            break;
        case 0x35:
            // DEC
            // 1  12
            // Z 1 H -
            break;
        case 0x36:
            // LD
            // 2  12
            // - - - -
            break;
        case 0x37:
            // SCF
            // 1  4
            // - 0 0 
            break;
        case 0x38:
            // JR C
            // 2  12
            // - - - -
            break;
        case 0x39:
            // ADD HL
            // 1  8
            // - 0 H C
            break;
        case 0x3A:
            // LD A
            // 1  8
            // - - - -
            break;
        case 0x3B:
            // DEC SP
            // 1  8
            // - - - -
            break;
        case 0x3C:
            // INC A
            // 1  4
            // Z 0 H -
            break;
        case 0x3D:
            // DEC A
            // 1  4
            // Z 1 H -
            break;
        case 0x3E:
            // LD A
            // 2  8
            // - - - -
            break;
        case 0x3F:
            // CCF
            // 1  4
            // - 0 0 C
            break;
        case 0x40:
            // LD B
            // 1  4
            // - - - -
            break;
        case 0x41:
            // LD B
            // 1  4
            // - - - -
            break;
        case 0x42:
            // LD B
            // 1  4
            // - - - -
            break;
        case 0x43:
            // LD B
            // 1  4
            // - - - -
            break;
        case 0x44:
            // LD B
            // 1  4
            // - - - -
            break;
        case 0x45:
            // LD B
            // 1  4
            // - - - -
            break;
        case 0x46:
            // LD B
            // 1  8
            // - - - -
            break;
        case 0x47:
            // LD B
            // 1  4
            // - - - -
            break;
        case 0x48:
            // LD C
            // 1  4
            // - - - -
            break;
        case 0x49:
            // LD C
            // 1  4
            // - - - -
            break;
        case 0x4A:
            // LD C
            // 1  4
            // - - - -
            break;
        case 0x4B:
            // LD C
            // 1  4
            // - - - -
            break;
        case 0x4C:
            // LD C
            // 1  4
            // - - - -
            break;
        case 0x4D:
            // LD C
            // 1  4
            // - - - -
            break;
        case 0x4E:
            // LD C
            // 1  8
            // - - - -
            break;
        case 0x4F:
            // LD C
            // 1  4
            // - - - -
            break;
        case 0x50:
            // LD D
            // 1  4
            // - - - -
            break;
        case 0x51:
            // LD D
            // 1  4
            // - - - -
            break;
        case 0x52:
            // LD D
            // 1  4
            // - - - -
            break;
        case 0x53:
            // LD D
            // 1  4
            // - - - -
            break;
        case 0x54:
            // LD D
            // 1  4
            // - - - -
            break;
        case 0x55:
            // LD D
            // 1  4
            // - - - -
            break;
        case 0x56:
            // LD D
            // 1  8
            // - - - -
            break;
        case 0x57:
            // LD D
            // 1  4
            // - - - -
            break;
        case 0x58:
            // LD E
            // 1  4
            // - - - -
            break;
        case 0x59:
            // LD E
            // 1  4
            // - - - -
            break;
        case 0x5A:
            // LD E
            // 1  4
            // - - - -
            break;
        case 0x5B:
            // LD E
            // 1  4
            // - - - -
            break;
        case 0x5C:
            // LD E
            // 1  4
            // - - - -
            break;
        case 0x5D:
            // LD E
            // 1  4
            // - - - -
            break;
        case 0x5E:
            // LD E
            // 1  8
            // - - - -
            break;
        case 0x5F:
            // LD E
            // 1  4
            // - - - -
            break;
        case 0x60:
            // LD H
            // 1  4
            // - - - -
            break;
        case 0x61:
            // LD H
            // 1  4
            // - - - -
            break;
        case 0x62:
            // LD H
            // 1  4
            // - - - -
            break;
        case 0x63:
            // LD H
            // 1  4
            // - - - -
            break;
        case 0x64:
            // LD H
            // 1  4
            // - - - -
            break;
        case 0x65:
            // LD H
            // 1  4
            // - - - -
            break;
        case 0x66:
            // LD H
            // 1  8
            // - - - -
            break;
        case 0x67:
            // LD H
            // 1  4
            // - - - -
            break;
        case 0x68:
            // LD L
            // 1  4
            // - - - -
            break;
        case 0x69:
            // LD L
            // 1  4
            // - - - -
            break;
        case 0x6A:
            // LD L
            // 1  4
            // - - - -
            break;
        case 0x6B:
            // LD L
            // 1  4
            // - - - -
            break;
        case 0x6C:
            // LD L
            // 1  4
            // - - - -
            break;
        case 0x6D:
            // LD L
            // 1  4
            // - - - -
            break;
        case 0x6E:
            // LD L
            // 1  8
            // - - - -
            break;
        case 0x6F:
            // LD L
            // 1  4
            // - - - -
            break;
        case 0x70:
            // LD
            // 1  8
            // - - - -
            break;
        case 0x71:
            // LD
            // 1  8
            // - - - -
            break;
        case 0x72:
            // LD
            // 1  8
            // - - - -
            break;
        case 0x73:
            // LD
            // 1  8
            // - - - -
            break;
        case 0x74:
            // LD
            // 1  8
            // - - - -
            break;
        case 0x75:
            // LD
            // 1  8
            // - - - -
            break;
        case 0x76:
            // HALT
            // 1  4
            // - - - -
            break;
        case 0x77:
            // LD
            // 1  8
            // - - - -
            break;
        case 0x78:
            // LD A
            // 1  4
            // - - - -
            break;
        case 0x79:
            // LD A
            // 1  4
            // - - - -
            break;
        case 0x7A:
            // LD A
            // 1  4
            // - - - -
            break;
        case 0x7B:
            // LD A
            // 1  4
            // - - - -
            break;
        case 0x7C:
            // LD A
            // 1  4
            // - - - -
            break;
        case 0x7D:
            // LD A
            // 1  4
            // - - - -
            break;
        case 0x7E:
            // LD A
            // 1  8
            // - - - -
            break;
        case 0x7F:
            // LD A
            // 1  4
            // - - - -
            break;
        case 0x80:
            // ADD A
            // 1  4
            // Z 0 H C
            break;
        case 0x81:
            // ADD A
            // 1  4
            // Z 0 H C
            break;
        case 0x82:
            // ADD A
            // 1  4
            // Z 0 H C
            break;
        case 0x83:
            // ADD A
            // 1  4
            // Z 0 H C
            break;
        case 0x84:
            // ADD A
            // 1  4
            // Z 0 H C
            break;
        case 0x85:
            // ADD A
            // 1  4
            // Z 0 H C
            break;
        case 0x86:
            // ADD A
            // 1  8
            // Z 0 H C
            break;
        case 0x87:
            // ADD A
            // 1  4
            // Z 0 H C
            break;
        case 0x88:
            // ADC A
            // 1  4
            // Z 0 H C
            break;
        case 0x89:
            // ADC A
            // 1  4
            // Z 0 H C
            break;
        case 0x8A:
            // ADC A
            // 1  4
            // Z 0 H C
            break;
        case 0x8B:
            // ADC A
            // 1  4
            // Z 0 H C
            break;
        case 0x8C:
            // ADC A
            // 1  4
            // Z 0 H C
            break;
        case 0x8D:
            // ADC A
            // 1  4
            // Z 0 H C
            break;
        case 0x8E:
            // ADC A
            // 1  8
            // Z 0 H C
            break;
        case 0x8F:
            // ADC A
            // 1  4
            // Z 0 H C
            break;
        case 0x90:
            // SUB B
            // 1  4
            // Z 1 H C
            break;
        case 0x91:
            // SUB C
            // 1  4
            // Z 1 H C
            break;
        case 0x92:
            // SUB D
            // 1  4
            // Z 1 H C
            break;
        case 0x93:
            // SUB E
            // 1  4
            // Z 1 H C
            break;
        case 0x94:
            // SUB H
            // 1  4
            // Z 1 H C
            break;
        case 0x95:
            // SUB L
            // 1  4
            // Z 1 H C
            break;
        case 0x96:
            // SUB
            // 1  8
            // Z 1 H C
            break;
        case 0x97:
            // SUB A
            // 1  4
            // Z 1 H C
            break;
        case 0x98:
            // SBC A
            // 1  4
            // Z 1 H C
            break;
        case 0x99:
            // SBC A
            // 1  4
            // Z 1 H C
            break;
        case 0x9A:
            // SBC A
            // 1  4
            // Z 1 H C
            break;
        case 0x9B:
            // SBC A
            // 1  4
            // Z 1 H C
            break;
        case 0x9C:
            // SBC A
            // 1  4
            // Z 1 H C
            break;
        case 0x9D:
            // SBC A
            // 1  4
            // Z 1 H C
            break;
        case 0x9E:
            // SBC A
            // 1  8
            // Z 1 H C
            break;
        case 0x9F:
            // SBC A
            // 1  4
            // Z 1 H C
            break;
        case 0xA0:
            // AND B
            // 1  4
            // Z 0 1 0
            break;
        case 0xA1:
            // AND C
            // 1  4
            // Z 0 1 0
            break;
        case 0xA2:
            // AND D
            // 1  4
            // Z 0 1 0
            break;
        case 0xA3:
            // AND E
            // 1  4
            // Z 0 1 0
            break;
        case 0xA4:
            // AND H
            // 1  4
            // Z 0 1 0
            break;
        case 0xA5:
            // AND L
            // 1  4
            // Z 0 1 0
            break;
        case 0xA6:
            // AND
            // 1  8
            // Z 0 1 0
            break;
        case 0xA7:
            // AND A
            // 1  4
            // Z 0 1 0
            break;
        case 0xA8:
            // XOR B
            // 1  4
            // Z 0 0 0
            break;
        case 0xA9:
            // XOR C
            // 1  4
            // Z 0 0 0
            break;
        case 0xAA:
            // XOR D
            // 1  4
            // Z 0 0 0
            break;
        case 0xAB:
            // XOR E
            // 1  4
            // Z 0 0 0
            break;
        case 0xAC:
            // XOR H
            // 1  4
            // Z 0 0 0
            break;
        case 0xAD:
            // XOR L
            // 1  4
            // Z 0 0 0
            break;
        case 0xAE:
            // XOR
            // 1  8
            // Z 0 0 0
            break;
        case 0xAF:
            // XOR A
            // 1  4
            // Z 0 0 0
            break;
        case 0xB0:
            // OR B
            // 1  4
            // Z 0 0 0
            break;
        case 0xB1:
            // OR C
            // 1  4
            // Z 0 0 0
            break;
        case 0xB2:
            // OR D
            // 1  4
            // Z 0 0 0
            break;
        case 0xB3:
            // OR E
            // 1  4
            // Z 0 0 0
            break;
        case 0xB4:
            // OR H
            // 1  4
            // Z 0 0 0
            break;
        case 0xB5:
            // OR L
            // 1  4
            // Z 0 0 0
            break;
        case 0xB6:
            // OR
            // 1  8
            // Z 0 0 0
            break;
        case 0xB7:
            // OR A
            // 1  4
            // Z 0 0 0
            break;
        case 0xB8:
            // CP B
            // 1  4
            // Z 1 H C
            break;
        case 0xB9:
            // CP C
            // 1  4
            // Z 1 H C
            break;
        case 0xBA:
            // CP D
            // 1  4
            // Z 1 H C
            break;
        case 0xBB:
            // CP E
            // 1  4
            // Z 1 H C
            break;
        case 0xBC:
            // CP H
            // 1  4
            // Z 1 H C
            break;
        case 0xBD:
            // CP L
            // 1  4
            // Z 1 H C
            break;
        case 0xBE:
            // CP
            // 1  8
            // Z 1 H C
            break;
        case 0xBF:
            // CP A
            // 1  4
            // Z 1 H C
            break;
        case 0xC0:
            // RET NZ
            // 1  20
            // - - - -
            break;
        case 0xC1:
            // POP BC
            // 1  12
            // - - - -
            break;
        case 0xC2:
            // JP NZ
            // 3  16
            // - - - -
            break;
        case 0xC3:
            // JP a163
            // 3  16
            // - - - -
            break;
        case 0xC4:
            // CALL NZ
            // 3  24
            // - - - -
            break;
        case 0xC5:
            // PUSH BC
            // 1  16
            // - - - -
            break;
        case 0xC6:
            // ADD A
            // 2  8
            // Z 0 H C
            break;
        case 0xC7:
            // RST 00H
            // 1  16
            // - - - -
            break;
        case 0xC8:
            // RET Z
            // 1  20
            // - - - -
            break;
        case 0xC9:
            // RET
            // 1  16
            // - - - -
            break;
        case 0xCA:
            // JP Z
            // 3  16
            // - - - -
            break;
        case 0xCB:
            // PREFIX CB
            // 1  4
            // - - - -
            break;
        case 0xCC:
            // CALL Z
            // 3  24
            // - - - -
            break;
        case 0xCD:
            // CALL a163
            // 3  24
            // - - - -
            break;
        case 0xCE:
            // ADC A
            // 2  8
            // Z 0 H C
            break;
        case 0xCF:
            // RST 08H
            // 1  16
            // - - - -
            break;
        case 0xD0:
            // RET NC
            // 1  20
            // - - - -
            break;
        case 0xD1:
            // POP DE
            // 1  12
            // - - - -
            break;
        case 0xD2:
            // JP NC
            // 3  16
            // - - - -
            break;
        case 0xD3:
            break;
        case 0xD4:
            // CALL NC
            // 3  24
            // - - - -
            break;
        case 0xD5:
            // PUSH DE
            // 1  16
            // - - - -
            break;
        case 0xD6:
            // SUB d82
            // 2  8
            // Z 1 H C
            break;
        case 0xD7:
            // RST 10H
            // 1  16
            // - - - -
            break;
        case 0xD8:
            // RET C
            // 1  20
            // - - - -
            break;
        case 0xD9:
            // RETI
            // 1  16
            // - - - -
            break;
        case 0xDA:
            // JP C
            // 3  16
            // - - - -
            break;
        case 0xDB:
            break;
        case 0xDC:
            // CALL C
            // 3  24
            // - - - -
            break;
        case 0xDD:
            break;
        case 0xDE:
            // SBC A
            // 2  8
            // Z 1 H C
            break;
        case 0xDF:
            // RST 18H
            // 1  16
            // - - - -
            break;
        case 0xE0:
            // LDH
            // 2  12
            // - - - -
            break;
        case 0xE1:
            // POP HL
            // 1  12
            // - - - -
            break;
        case 0xE2:
            // LD
            // 2  8
            // - - - -
            break;
        case 0xE3:
            break;
        case 0xE4:
            break;
        case 0xE5:
            // PUSH HL
            // 1  16
            // - - - -
            break;
        case 0xE6:
            // AND d82
            // 2  8
            // Z 0 1 0
            break;
        case 0xE7:
            // RST 20H
            // 1  16
            // - - - -
            break;
        case 0xE8:
            // ADD SP
            // 2  160
            // 0 0 H C
            break;
        case 0xE9:
            // JP
            // 1  4
            // - - - -
            break;
        case 0xEA:
            // LD
            // 3  16
            // - - - -
            break;
        case 0xEB:
            break;
        case 0xEC:
            break;
        case 0xED:
            break;
        case 0xEE:
            // XOR d82
            // 2  8
            // Z 0 0 0
            break;
        case 0xEF:
            // RST 28H'
            // 1  16
            // - - - -
            break;
        case 0xF0:
            // LDH A
            // 2  12
            // - - - -
            break;
        case 0xF1:
            // POP AF'
            // 1  12
            // Z N H C
            break;
        case 0xF2:
            // LD A
            // 2  8
            // - - - -
            break;
        case 0xF3:
            // DI
            // 1  4
            // - - - -
            break;
        case 0xF4:
            break;
        case 0xF5:
            // PUSH AF
            // 1  16
            // - - - -
            break;
        case 0xF6:
            // OR d82
            // 2  8
            // Z 0 0 0
            break;
        case 0xF7:
            // RST 30H
            // 1  16
            // - - - -
            break;
        case 0xF8:
            // LD HL
            // 2  120
            // 0 0 H C
            break;
        case 0xF9:
            // LD SP
            // 1  8
            // - - - -
            break;
        case 0xFA:
            // LD A
            // 3  16
            // - - - -
            break;
        case 0xFB:
            // EI
            // 1  4
            // - - - -
            break;
        case 0xFC:
            break;
        case 0xFD:
            break;
        case 0xFE:
            // CP d82
            // 2  8
            // Z 1 H C
            break;
        case 0xFF:
            // RST 38H
            // 1  16
            // - - - -
            break;
    }
}