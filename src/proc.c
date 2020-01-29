// proc.c
#include "proc.h"
#include "memory.h"

#define RESET_ZERO p->flagRegister.zero = CLEAR;
#define SET_ZERO p->flagRegister.zero = SET;

#define RESET_SUBTRACT p->flagRegister.subtract = CLEAR;
#define SET_SUBTRACT p->flagRegister.subtract = SET;

#define RESET_HALF_CARRY p->flagRegister.half_carry = CLEAR;
#define SET_HALF_CARRY p->flagRegister.half_carry = SET;

#define RESET_CARRY p->flagRegister.carry = CLEAR;
#define SET_CARRY p->flagRegister.carry = SET;

#define CHECK_AND_SET_ZERO(x) p->flagRegister.zero = (x == 0);

#define DECREMENT_AND_CHECK(x) \
    if (is_half_carry_sub(x, 1)) {\
        SET_HALF_CARRY;\
    }\
    x --;

#define INCREMENT_AND_CHECK(x) \
    if (is_half_carry_add(x, 1)) {\
        SET_HALF_CARRY;\
    }\
    x ++;

Proc* proc_create() {
    Proc* p = calloc(1, sizeof(Proc));
    if (p) {
        p->pc = 0x100;
        p->sp = 0xFFFE;
    }
    proc_initialize_memory(p);
    return p;
}

void proc_delete(Proc* p) {
    if (!p) return;
    free(p);
}

void proc_read_word(Proc *p) {
    if (!p) return;

    uint8_t eightbit_opcode = p->memory[p->pc];
    /* set a variable instead of just ++ -- to account for changing PC value as inst */
    int bytes_ate = 1;

    /* the next 8 bits of data */
    uint8_t d8 = 0;

    uint8_t value_in_memory = 0;

    uint16_t combined_value = 0;

    /* https:/www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html */
    /* LITTLE ENDIAN I THINK */
    switch (eightbit_opcode) {
        case 0x0:
            //  NOP
            // 1 4
            // - - - -
            debug_print("NOP\n", NULL);
			break;
        case 0x1:
            // LD BC,d16
            // 3 12
            // - - - -
            debug_print("LD BC,d16\n", NULL);
            /* Assuming B is most significant... so c gets first byte and B gets second */
            p->registers.c = p->memory[p->pc + 1];
            p->registers.b = p->memory[p->pc + 2];
            bytes_ate = 3;
			break;
        case 0x2:
            // LD (BC),A
            // 1 8
            // - - - -
            debug_print("LD (BC),A\n", NULL);
            write_byte(p, (p->registers.b << 8) + p->registers.c, p->registers.a);
			break;
        case 0x3:
            // INC BC
            // 1 8
            // - - - -
            debug_print("INC BC\n", NULL);
            combined_value = get_16bit_value(p->registers.b, p->registers.c);
            combined_value ++;
            p->registers.b = get_upper_8bit_value(combined_value);
            p->registers.c = get_lower_8bit_value(combined_value);
			break;
        case 0x4:
            // INC B
            // 1 4
            // Z 0 H -
            RESET_SUBTRACT;

            debug_print("INC B\n", NULL);
            INCREMENT_AND_CHECK(p->registers.b);
            CHECK_AND_SET_ZERO(p->registers.b);
			break;
        case 0x5:
            // DEC B
            // 1 4
            // Z 1 H -
            SET_SUBTRACT;

            debug_print("DEC B\n", NULL);
            DECREMENT_AND_CHECK(p->registers.b);
            CHECK_AND_SET_ZERO(p->registers.b);
			break;
        case 0x6:
            // LD B,d8
            // 2 8
            // - - - -
            p->registers.b = p->memory[p->pc + 1];
            bytes_ate = 2;
            debug_print("LD B,d8\n", NULL);
			break;
        case 0x7: {
            //  RLCA
            // 1 4
            // 0 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;

            // grab carry bit
            uint8_t c = p->registers.a >> 7;
            // shift left 1 and drop carry on thurrr
            p->registers.a = (p->registers.a << 1) | c;

            // set extra flags
            p->flagRegister.carry = c;
            p->flagRegister.zero = (p->registers.a == 0);

            debug_print("RLCA\n", NULL);
			break;
        }
        case 0x8:
            // LD (a16),SP
            // 3 20
            // - - - -
            debug_print("LD (a16),SP\n", NULL);
            write_byte(
                p,
                p->memory[p->pc + 1] + (p->memory[p->pc + 2] << 8),
                p->sp
            );
			break;
        case 0x9:
            // ADD HL,BC
            // 1 8
            // - 0 H C
            RESET_SUBTRACT;
            debug_print("ADD HL,BC\n", NULL);
			break;
        case 0xA:
            // LD A,(BC)
            // 1 8
            // - - - -
            p->registers.a = p->memory[p->registers.c + (p->registers.b << 8)];
            debug_print("LD A,(BC)\n", NULL);
			break;
        case 0xB:
            // DEC BC
            // 1 8
            // - - - -
            debug_print("DEC BC\n", NULL);
            combined_value = get_16bit_value(p->registers.b, p->registers.c);
            combined_value --;
            p->registers.b = get_upper_8bit_value(combined_value);
            p->registers.c = get_lower_8bit_value(combined_value);
			break;
        case 0xC:
            // INC C
            // 1 4
            // Z 0 H -
            RESET_SUBTRACT;

            debug_print("INC C\n", NULL);
            INCREMENT_AND_CHECK(p->registers.c);
            CHECK_AND_SET_ZERO(p->registers.c);
			break;
        case 0xD:
            // DEC C
            // 1 4
            // Z 1 H -
            SET_SUBTRACT;

            debug_print("DEC C\n", NULL);
            DECREMENT_AND_CHECK(p->registers.c);
            CHECK_AND_SET_ZERO(p->registers.c);
			break;
        case 0xE:
            // LD C,d8
            // 2 8
            // - - - -
            p->registers.c = p->memory[p->pc + 1];
            bytes_ate = 2;
            debug_print("LD C,d8\n", NULL);
			break;
        case 0xF: {
            //  RRCA
            // 1 4
            // 0 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;

            uint8_t c = p->registers.a & 0x01; // right bit
            p->registers.a = (p->registers.a >> 1) | (c << 7);

            p->flagRegister.zero  = (p->registers.a == 0);
            p->flagRegister.carry = c;
            debug_print("RRCA\n", NULL);
			break;
        }
        case 0x10:
            // STOP 0
            // 2 4
            // - - - -
            debug_print("STOP 0\n", NULL);
			break;
        case 0x11:
            // LD DE,d16
            // 3 12
            // - - - -
            p->registers.e = p->memory[p->pc + 1];
            p->registers.d = p->memory[p->pc + 2];
            bytes_ate = 3;
            debug_print("LD DE,d16\n", NULL);
			break;
        case 0x12:
            // LD (DE),A
            // 1 8
            // - - - -
            write_byte(p, (p->registers.d << 8) + p->registers.e, p->registers.a);
            debug_print("LD (DE),A\n", NULL);
	    break;
        case 0x13:
            // INC DE
            // 1 8
            // - - - -
            debug_print("INC DE\n", NULL);
            combined_value = get_16bit_value(p->registers.d, p->registers.e);
            combined_value ++;
            p->registers.d = get_upper_8bit_value(combined_value);
            p->registers.e = get_lower_8bit_value(combined_value);
			break;
        case 0x14:
            // INC D
            // 1 4
            // Z 0 H -
            RESET_SUBTRACT;

            debug_print("INC D\n", NULL);
            INCREMENT_AND_CHECK(p->registers.d);
            CHECK_AND_SET_ZERO(p->registers.d);
			break;
        case 0x15:
            // DEC D
            // 1 4
            // Z 1 H -
            SET_SUBTRACT;

            debug_print("DEC D\n", NULL);
            DECREMENT_AND_CHECK(p->registers.d);
            CHECK_AND_SET_ZERO(p->registers.d);
			break;
        case 0x16:
            // LD D,d8
            // 2 8
            // - - - -
            p->registers.d = p->memory[p->pc + 1];
            bytes_ate = 2;
            debug_print("LD D,d8\n", NULL);
			break;
        case 0x17: {
            //  RLA
            // 1 4
            // 0 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            // shift to get carry
            uint8_t c = p->registers.a >> 7;
            // left 1 and then move old carry bit into bit 0
            p->registers.a = (p->registers.a << 1) | p->flagRegister.carry;

            p->flagRegister.zero = (p->registers.a == 0);
            p->flagRegister.carry = c;
            debug_print("RLA\n", NULL);
			break;
        }
        case 0x18:
            // JR r8
            // 2 12
            // - - - -
            debug_print("JR r8\n", NULL);
            p->pc += (int8_t) p->memory[p->pc + 1];
            bytes_ate = 0;
			break;
        case 0x19:
            // ADD HL,DE
            // 1 8
            // - 0 H C
            RESET_SUBTRACT;

            debug_print("ADD HL,DE\n", NULL);
			break;
        case 0x1A:
            // LD A,(DE)
            // 1 8
            // - - - -
            debug_print("LD A,(DE)\n", NULL);
            p->registers.a = p->memory[p->registers.e + (p->registers.d << 8)];
			break;
        case 0x1B:
            // DEC DE
            // 1 8
            // - - - -
            debug_print("DEC DE\n", NULL);
            combined_value = get_16bit_value(p->registers.d, p->registers.e);
            combined_value --;
            p->registers.d = get_upper_8bit_value(combined_value);
            p->registers.e = get_lower_8bit_value(combined_value);
			break;
        case 0x1C:
            // INC E
            // 1 4
            // Z 0 H -
            RESET_SUBTRACT;

            debug_print("INC E\n", NULL);
            INCREMENT_AND_CHECK(p->registers.e);
            CHECK_AND_SET_ZERO(p->registers.e);
			break;
        case 0x1D:
            // DEC E
            // 1 4
            // Z 1 H -
            SET_SUBTRACT;

            debug_print("DEC E\n", NULL);
            DECREMENT_AND_CHECK(p->registers.e);
            CHECK_AND_SET_ZERO(p->registers.e);
			break;
        case 0x1E:
            // LD E,d8
            // 2 8
            // - - - -
            p->registers.e = p->memory[p->pc + 1];
            bytes_ate = 2;
            debug_print("LD E,d8\n", NULL);
			break;
        case 0x1F: {
            //  RRA
            // 1 4
            // 0 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;

            uint8_t      c = p->registers.a & 0x01;
            p->registers.a = (p->registers.a >> 1) | (p->flagRegister.carry << 7);

            p->flagRegister.zero  = (p->registers.a == 0);
            p->flagRegister.carry = c;
            debug_print("RRA\n", NULL);
			break;
        }
        case 0x20:
            // JR NZ,r8
            // 2 12/8
            // - - - -
            debug_print("JR NZ,r8\n", NULL);
            if (!p->flagRegister.zero) {
                p->pc += (int8_t)p->memory[p->pc + 1];
                bytes_ate = 0;
            }
			break;
        case 0x21:
            // LD HL,d16
            // 3 12
            // - - - -
            p->registers.l = p->memory[p->pc + 1];
            p->registers.h = p->memory[p->pc + 2];
            bytes_ate = 3;
            debug_print("LD HL,d16\n", NULL);
			break;
        case 0x22:
            // LD (HL+),A
            // 1 8
            // - - - -
            debug_print("LD (HL+),A\n", NULL);
            write_byte(p, (p->registers.h << 8) + p->registers.l, p->registers.a);
            proc_inc_hl(p);
			break;
        case 0x23:
            // INC HL
            // 1 8
            // - - - -
            debug_print("INC HL\n", NULL);
            combined_value = get_16bit_value(p->registers.h, p->registers.l);
            combined_value ++;
            p->registers.h = get_upper_8bit_value(combined_value);
            p->registers.l = get_lower_8bit_value(combined_value);
			break;
        case 0x24:
            // INC H
            // 1 4
            // Z 0 H -
            RESET_SUBTRACT;

            debug_print("INC H\n", NULL);
            INCREMENT_AND_CHECK(p->registers.h);
            CHECK_AND_SET_ZERO(p->registers.h);
			break;
        case 0x25:
            // DEC H
            // 1 4
            // Z 1 H -
            SET_SUBTRACT;

            debug_print("DEC H\n", NULL);
            DECREMENT_AND_CHECK(p->registers.h);
            CHECK_AND_SET_ZERO(p->registers.h);
			break;
        case 0x26:
            // LD H,d8
            // 2 8
            // - - - -
            p->registers.h = p->memory[p->pc + 1];
            bytes_ate = 2;
            debug_print("LD H,d8\n", NULL);
			break;
        case 0x27:
            //  DAA
            // 1 4
            // Z - 0 C
            RESET_HALF_CARRY;

            debug_print("DAA\n", NULL);
			break;
        case 0x28:
            // JR Z,r8
            // 2 12/8
            // - - - -
            debug_print("JR Z,r8\n", NULL);
            if (p->flagRegister.zero) {
                p->pc += (int8_t) p->memory[p->pc + 1];
                bytes_ate = 0;
            }
			break;
        case 0x29:
            // ADD HL,HL
            // 1 8
            // - 0 H C
            RESET_SUBTRACT;

            debug_print("ADD HL,HL\n", NULL);
			break;
        case 0x2A:
            // LD A,(HL+)
            // 1 8
            // - - - -
            debug_print("LD A,(HL+)\n", NULL);
            p->registers.a = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)];
            proc_inc_hl(p);
			break;
        case 0x2B:
            // DEC HL
            // 1 8
            // - - - -
            debug_print("DEC HL\n", NULL);
            combined_value = get_16bit_value(p->registers.h, p->registers.l);
            combined_value --;
            p->registers.h = get_upper_8bit_value(combined_value);
            p->registers.l = get_lower_8bit_value(combined_value);
			break;
        case 0x2C:
            // INC L
            // 1 4
            // Z 0 H -
            RESET_SUBTRACT;

            debug_print("INC L\n", NULL);
            INCREMENT_AND_CHECK(p->registers.l);
            CHECK_AND_SET_ZERO(p->registers.l);
			break;
        case 0x2D:
            // DEC L
            // 1 4
            // Z 1 H -
            SET_SUBTRACT;

            debug_print("DEC L\n", NULL);
            DECREMENT_AND_CHECK(p->registers.l);
            CHECK_AND_SET_ZERO(p->registers.l);
			break;
        case 0x2E:
            // LD L,d8
            // 2 8
            // - - - -
            p->registers.l = p->memory[p->pc + 1];
            bytes_ate = 2;
            debug_print("LD L,d8\n", NULL);
			break;
        case 0x2F:
            //  CPL
            // 1 4
            // - 1 1 -
            SET_SUBTRACT;
            SET_HALF_CARRY;
            p->registers.a = ~p->registers.a;
            debug_print("CPL\n", NULL);
			break;
        case 0x30:
            // JR NC,r8
            // 2 12/8
            // - - - -
            debug_print("JR NC,r8\n", NULL);
            if (!p->flagRegister.carry) {
                p->pc += (int8_t) p->memory[p->pc + 1];
                bytes_ate = 0;
            }
			break;
        case 0x31:
            // LD SP,d16
            // 3 12
            // - - - -
            debug_print("LD SP,d16\n", NULL);
            p->sp = p->memory[p->pc + 1] + (p->memory[p->pc + 2] << 8);
            bytes_ate = 3;
			break;
        case 0x32:
            // LD (HL-),A
            // 1 8
            // - - - -
            debug_print("LD (HL-),A\n", NULL);
            write_byte(p, (p->registers.h << 8) + p->registers.l, p->registers.a);
            proc_dec_hl(p);
			break;
        case 0x33:
            // INC SP
            // 1 8
            // - - - -
            debug_print("INC SP\n", NULL);
            p->sp ++;
			break;
        case 0x34:
            // INC (HL)
            // 1 12
            // Z 0 H -
            RESET_SUBTRACT;
            combined_value = get_16bit_value(p->registers.h, p->registers.l);
            value_in_memory = p->memory[combined_value];
            INCREMENT_AND_CHECK(value_in_memory);
            write_byte(p, combined_value, value_in_memory);
            debug_print("INC (HL)\n", NULL);
			break;
        case 0x35:
            // DEC (HL)
            // 1 12
            // Z 1 H -
            SET_SUBTRACT;
            combined_value = get_16bit_value(p->registers.h, p->registers.l);
            value_in_memory = p->memory[combined_value];
            DECREMENT_AND_CHECK(value_in_memory);
            write_byte(p, combined_value, value_in_memory);
            debug_print("DEC (HL)\n", NULL);
			break;
        case 0x36:
            // LD (HL),d8
            // 2 12
            // - - - -
            debug_print("LD (HL),d8\n", NULL);
            write_byte(p, (p->registers.h << 8) + p->registers.l, p->memory[p->pc + 1]);
            bytes_ate = 2;
			break;
        case 0x37:
            //  SCF
            // 1 4
            // - 0 0 1
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            SET_CARRY;
            debug_print("SCF\n", NULL);
			break;
        case 0x38:
            // JR C,r8
            // 2 12/8
            // - - - -
            debug_print("JR C,r8\n", NULL);
            if (p->flagRegister.carry) {
                p->pc += (int8_t) p->memory[p->pc + 1];
                bytes_ate = 0;
            }
			break;
        case 0x39:
            // ADD HL,SP
            // 1 8
            // - 0 H C
            debug_print("ADD HL,SP\n", NULL);
			break;
        case 0x3A:
            // LD A,(HL-)
            // 1 8
            // - - - -
            debug_print("LD A,(HL-)\n", NULL);
            p->registers.a = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)];
            proc_dec_hl(p);
            break;
        case 0x3B:
            // DEC SP
            // 1 8
            // - - - -
            debug_print("DEC SP\n", NULL);
            p->sp --;
			break;
        case 0x3C:
            // INC A
            // 1 4
            // Z 0 H -
            RESET_SUBTRACT;
            debug_print("INC A\n", NULL);
            INCREMENT_AND_CHECK(p->registers.a);
            CHECK_AND_SET_ZERO(p->registers.a);
			break;
        case 0x3D:
            // DEC A
            // 1 4
            // Z 1 H -
            SET_SUBTRACT;

            debug_print("DEC A\n", NULL);
            DECREMENT_AND_CHECK(p->registers.a);
            CHECK_AND_SET_ZERO(p->registers.a);
			break;
        case 0x3E:
            // LD A,d8
            // 2 8
            // - - - -
            debug_print("LD A,d8\n", NULL);
            p->registers.a = p->memory[p->pc + 1];
            bytes_ate = 2;
			break;
        case 0x3F:
            //  CCF
            // 1 4
            // - 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            p->flagRegister.carry = !p->flagRegister.carry;
            debug_print("CCF\n", NULL);
			break;
        case 0x40:
            // LD B,B
            // 1 4
            // - - - -
            p->registers.b = p->registers.b;
            debug_print("LD B,B\n", NULL);
			break;
        case 0x41:
            // LD B,C
            // 1 4
            // - - - -
            p->registers.b = p->registers.c;
            debug_print("LD B,C\n", NULL);
			break;
        case 0x42:
            // LD B,D
            // 1 4
            // - - - -
            p->registers.b = p->registers.d;
            debug_print("LD B,D\n", NULL);
			break;
        case 0x43:
            // LD B,E
            // 1 4
            // - - - -
            p->registers.b = p->registers.e;
            debug_print("LD B,E\n", NULL);
			break;
        case 0x44:
            // LD B,H
            // 1 4
            // - - - -
            debug_print("LD B,H\n", NULL);
			break;
        case 0x45:
            // LD B,L
            // 1 4
            // - - - -
            p->registers.b = p->registers.l;
            debug_print("LD B,L\n", NULL);
			break;
        case 0x46:
            // LD B,(HL)
            // 1 8
            // - - - -
            p->registers.b = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)];
            debug_print("LD B,(HL)\n", NULL);
			break;
        case 0x47:
            // LD B,A
            // 1 4
            // - - - -
            p->registers.b = p->registers.a;
            debug_print("LD B,A\n", NULL);
			break;
        case 0x48:
            // LD C,B
            // 1 4
            // - - - -
            p->registers.c = p->registers.b;
            debug_print("LD C,B\n", NULL);
			break;
        case 0x49:
            // LD C,C
            // 1 4
            // - - - -
            p->registers.c = p->registers.c;
            debug_print("LD C,C\n", NULL);
			break;
        case 0x4A:
            // LD C,D
            // 1 4
            // - - - -
            p->registers.c = p->registers.d;
            debug_print("LD C,D\n", NULL);
			break;
        case 0x4B:
            // LD C,E
            // 1 4
            // - - - -
            p->registers.c = p->registers.e;
            debug_print("LD C,E\n", NULL);
			break;
        case 0x4C:
            // LD C,H
            // 1 4
            // - - - -
            p->registers.c = p->registers.h;
            debug_print("LD C,H\n", NULL);
			break;
        case 0x4D:
            // LD C,L
            // 1 4
            // - - - -
            p->registers.c = p->registers.l;
            debug_print("LD C,L\n", NULL);
			break;
        case 0x4E:
            // LD C,(HL)
            // 1 8
            // - - - -
            p->registers.c = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)];
            debug_print("LD C,(HL)\n", NULL);
			break;
        case 0x4F:
            // LD C,A
            // 1 4
            // - - - -
            p->registers.c = p->registers.a;
            debug_print("LD C,A\n", NULL);
			break;
        case 0x50:
            // LD D,B
            // 1 4
            // - - - -
            p->registers.d = p->registers.b;
            debug_print("LD D,B\n", NULL);
			break;
        case 0x51:
            // LD D,C
            // 1 4
            // - - - -
            p->registers.d = p->registers.c;
            debug_print("LD D,C\n", NULL);
			break;
        case 0x52:
            // LD D,D
            // 1 4
            // - - - -
            p->registers.d = p->registers.d;
            debug_print("LD D,D\n", NULL);
			break;
        case 0x53:
            // LD D,E
            // 1 4
            // - - - -
            p->registers.d = p->registers.e;
            debug_print("LD D,E\n", NULL);
			break;
        case 0x54:
            // LD D,H
            // 1 4
            // - - - -
            p->registers.d = p->registers.h;
            debug_print("LD D,H\n", NULL);
			break;
        case 0x55:
            // LD D,L
            // 1 4
            // - - - -
            p->registers.d = p->registers.l;
            debug_print("LD D,L\n", NULL);
			break;
        case 0x56:
            // LD D,(HL)
            // 1 8
            // - - - -
            p->registers.d = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)];
            debug_print("LD D,(HL)\n", NULL);
			break;
        case 0x57:
            // LD D,A
            // 1 4
            // - - - -
            debug_print("LD D,A\n", NULL);
            p->registers.d = p->registers.a;
			break;
        case 0x58:
            // LD E,B
            // 1 4
            // - - - -
            p->registers.e = p->registers.b;
            debug_print("LD E,B\n", NULL);
			break;
        case 0x59:
            // LD E,C
            // 1 4
            // - - - -
            p->registers.e = p->registers.c;
            debug_print("LD E,C\n", NULL);
			break;
        case 0x5A:
            // LD E,D
            // 1 4
            // - - - -
            p->registers.e = p->registers.d;
            debug_print("LD E,D\n", NULL);
			break;
        case 0x5B:
            // LD E,E
            // 1 4
            // - - - -
            p->registers.e = p->registers.e;
            debug_print("LD E,E\n", NULL);
			break;
        case 0x5C:
            // LD E,H
            // 1 4
            // - - - -
            p->registers.e = p->registers.h;
            debug_print("LD E,H\n", NULL);
			break;
        case 0x5D:
            // LD E,L
            // 1 4
            // - - - -
            p->registers.e = p->registers.l;
            debug_print("LD E,L\n", NULL);
			break;
        case 0x5E:
            // LD E,(HL)
            // 1 8
            // - - - -
            p->registers.e = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)];
            debug_print("LD E,(HL)\n", NULL);
			break;
        case 0x5F:
            // LD E,A
            // 1 4
            // - - - -
            p->registers.e = p->registers.a;
            debug_print("LD E,A\n", NULL);
			break;
        case 0x60:
            // LD H,B
            // 1 4
            // - - - -
            p->registers.h = p->registers.b;
            debug_print("LD H,B\n", NULL);
			break;
        case 0x61:
            // LD H,C
            // 1 4
            // - - - -
            p->registers.h = p->registers.c;
            debug_print("LD H,C\n", NULL);
			break;
        case 0x62:
            // LD H,D
            // 1 4
            // - - - -
            p->registers.h = p->registers.d;
            debug_print("LD H,D\n", NULL);
			break;
        case 0x63:
            // LD H,E
            // 1 4
            // - - - -
            p->registers.h = p->registers.e;
            debug_print("LD H,E\n", NULL);
			break;
        case 0x64:
            // LD H,H
            // 1 4
            // - - - -
            p->registers.h = p->registers.h;
            debug_print("LD H,H\n", NULL);
			break;
        case 0x65:
            // LD H,L
            // 1 4
            // - - - -
            p->registers.h = p->registers.l;
            debug_print("LD H,L\n", NULL);
			break;
        case 0x66:
            // LD H,(HL)
            // 1 8
            // - - - -
            p->registers.h = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)];
            debug_print("LD H,(HL)\n", NULL);
			break;
        case 0x67:
            // LD H,A
            // 1 4
            // - - - -
            p->registers.h = p->registers.a;
            debug_print("LD H,A\n", NULL);
			break;
        case 0x68:
            // LD L,B
            // 1 4
            // - - - -
            p->registers.l = p->registers.b;
            debug_print("LD L,B\n", NULL);
			break;
        case 0x69:
            // LD L,C
            // 1 4
            // - - - -
            p->registers.l = p->registers.c;
            debug_print("LD L,C\n", NULL);
			break;
        case 0x6A:
            // LD L,D
            // 1 4
            // - - - -
            debug_print("LD L,D\n", NULL);
            p->registers.l = p->registers.d;
			break;
        case 0x6B:
            // LD L,E
            // 1 4
            // - - - -
            debug_print("LD L,E\n", NULL);
            p->registers.l = p->registers.e;
			break;
        case 0x6C:
            // LD L,H
            // 1 4
            // - - - -
            debug_print("LD L,H\n", NULL);
            p->registers.l = p->registers.h;
			break;
        case 0x6D:
            // LD L,L
            // 1 4
            // - - - -
            debug_print("LD L,L\n", NULL);
            p->registers.l = p->registers.l;
			break;
        case 0x6E:
            // LD L,(HL)
            // 1 8
            // - - - -
            p->registers.l = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)];
            debug_print("LD L,(HL)\n", NULL);
			break;
        case 0x6F:
            // LD L,A
            // 1 4
            // - - - -
            debug_print("LD L,A\n", NULL);
            p->registers.l = p->registers.a;
			break;
        case 0x70:
            // LD (HL),B
            // 1 8
            // - - - -
            write_byte(p, p->registers.l + (p->registers.h << 8), p->registers.b);
            debug_print("LD (HL),B\n", NULL);
			break;
        case 0x71:
            // LD (HL),C
            // 1 8
            // - - - -
            write_byte(p, p->registers.l + (p->registers.h << 8), p->registers.c);
            debug_print("LD (HL),C\n", NULL);
			break;
        case 0x72:
            // LD (HL),D
            // 1 8
            // - - - -
            write_byte(p, p->registers.l + (p->registers.h << 8), p->registers.d);
            debug_print("LD (HL),D\n", NULL);
			break;
        case 0x73:
            // LD (HL),E
            // 1 8
            // - - - -
            write_byte(p, p->registers.l + (p->registers.h << 8), p->registers.e);
            debug_print("LD (HL),E\n", NULL);
			break;
        case 0x74:
            // LD (HL),H
            // 1 8
            // - - - -
            write_byte(p, p->registers.l + (p->registers.h << 8), p->registers.h);
            debug_print("LD (HL),H\n", NULL);
			break;
        case 0x75:
            // LD (HL),L
            // 1 8
            // - - - -
            write_byte(p, p->registers.l + (p->registers.h << 8), p->registers.l);
            debug_print("LD (HL),L\n", NULL);
			break;
        case 0x76:
            //  HALT
            // 1 4
            // - - - -
            debug_print("HALT\n", NULL);
			break;
        case 0x77:
            // LD (HL),A
            // 1 8
            // - - - -
            debug_print("LD (HL),A\n", NULL);
            write_byte(p, p->registers.l + (p->registers.h << 8), p->registers.a);
			break;
        case 0x78:
            // LD A,B
            // 1 4
            // - - - -
            debug_print("LD A,B\n", NULL);
            p->registers.a = p->registers.b;
			break;
        case 0x79:
            // LD A,C
            // 1 4
            // - - - -
            debug_print("LD A,C\n", NULL);
            p->registers.a = p->registers.c;
			break;
        case 0x7A:
            // LD A,D
            // 1 4
            // - - - -
            debug_print("LD A,D\n", NULL);
            p->registers.a = p->registers.d;
			break;
        case 0x7B:
            // LD A,E
            // 1 4
            // - - - -
            debug_print("LD A,E\n", NULL);
            p->registers.a = p->registers.e;
			break;
        case 0x7C:
            // LD A,H
            // 1 4
            // - - - -
            debug_print("LD A,H\n", NULL);
            p->registers.a = p->registers.h;
			break;
        case 0x7D:
            // LD A,L
            // 1 4
            // - - - -
            debug_print("LD A,L\n", NULL);
            p->registers.a = p->registers.l;
			break;
        case 0x7E:
            // LD A,(HL)
            // 1 8
            // - - - -
            p->registers.a = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)];
            debug_print("LD A,(HL)\n", NULL);
			break;
        case 0x7F:
            // LD A,A
            // 1 4
            // - - - -
            p->registers.a = p->registers.a;
            debug_print("LD A,A\n", NULL);
			break;
        case 0x80:
            // ADD A,B
            // 1 4
            // Z 0 H C
            debug_print("ADD A,B\n", NULL);
            p->flagRegister.half_carry = is_half_carry_add(p->registers.a, p->registers.b);
            p->flagRegister.carry = 0xFF < ((uint16_t) p->registers.a + (uint16_t) p->registers.b);

            p->registers.a += p->registers.b;

			CHECK_AND_SET_ZERO(p->registers.a);
            RESET_SUBTRACT;
			break;
        case 0x81:
            // ADD A,C
            // 1 4
            // Z 0 H C
            debug_print("ADD A,C\n", NULL);
            p->flagRegister.half_carry = is_half_carry_add(p->registers.a, p->registers.c);
            p->flagRegister.carry = 0xFF < ((uint16_t) p->registers.a + (uint16_t) p->registers.c);

            p->registers.a += p->registers.c;

			CHECK_AND_SET_ZERO(p->registers.a);
            RESET_SUBTRACT;
			break;
        case 0x82:
            // ADD A,D
            // 1 4
            // Z 0 H C
            debug_print("ADD A,D\n", NULL);
            p->flagRegister.half_carry = is_half_carry_add(p->registers.a, p->registers.d);
            p->flagRegister.carry = 0xFF < ((uint16_t) p->registers.a + (uint16_t) p->registers.d);

            p->registers.a += p->registers.d;

			CHECK_AND_SET_ZERO(p->registers.a);
            RESET_SUBTRACT;
			break;
        case 0x83:
            // ADD A,E
            // 1 4
            // Z 0 H C
            debug_print("ADD A,E\n", NULL);
            p->flagRegister.half_carry = is_half_carry_add(p->registers.a, p->registers.e);
            p->flagRegister.carry = 0xFF < ((uint16_t) p->registers.a + (uint16_t) p->registers.e);

            p->registers.a += p->registers.e;

			CHECK_AND_SET_ZERO(p->registers.a);
            RESET_SUBTRACT;
			break;
        case 0x84:
            // ADD A,H
            // 1 4
            // Z 0 H C
            debug_print("ADD A,H\n", NULL);
            p->flagRegister.half_carry = is_half_carry_add(p->registers.a, p->registers.h);
            p->flagRegister.carry = 0xFF < ((uint16_t) p->registers.a + (uint16_t) p->registers.h);

            p->registers.a += p->registers.h;

			CHECK_AND_SET_ZERO(p->registers.a);
            RESET_SUBTRACT;
			break;
        case 0x85:
            // ADD A,L
            // 1 4
            // Z 0 H C
            debug_print("ADD A,L\n", NULL);
            p->flagRegister.half_carry = is_half_carry_add(p->registers.a, p->registers.l);
            p->flagRegister.carry = 0xFF < ((uint16_t) p->registers.a + (uint16_t) p->registers.l);

            p->registers.a += p->registers.l;

			CHECK_AND_SET_ZERO(p->registers.a);
            RESET_SUBTRACT;
			break;
        case 0x86: {
            // ADD A,(HL)
            // 1 8
            // Z 0 H C
            debug_print("ADD A,(HL)\n", NULL);
            uint8_t val = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)];
            p->flagRegister.half_carry = is_half_carry_add(p->registers.a, val);
            p->flagRegister.carry = 0xFF < ((uint16_t) p->registers.a + (uint16_t) val);

            p->registers.a += val;

			CHECK_AND_SET_ZERO(p->registers.a);
            RESET_SUBTRACT;
			break;
        }
        case 0x87:
            // ADD A,A
            // 1 4
            // Z 0 H C
            debug_print("ADD A,A\n", NULL);
            p->flagRegister.half_carry = is_half_carry_add(p->registers.a, p->registers.a);
            p->flagRegister.carry = 0xFF < ((uint16_t) p->registers.a + (uint16_t) p->registers.a);

            p->registers.a += p->registers.a;

			CHECK_AND_SET_ZERO(p->registers.a);
            RESET_SUBTRACT;
            break;
        case 0x88: {
            // ADC A,B
            // 1 4
            // Z 0 H C
            uint8_t result = p->registers.a + p->registers.b + p->flagRegister.carry;
            p->flagRegister.half_carry = is_three_half_carry_add(p->registers.a, p->registers.b, p->flagRegister.carry);
            p->flagRegister.carry      = 0xFF < ((uint16_t) p->registers.a + (uint16_t) p->registers.b + (uint16_t) p->flagRegister.carry);
            p->registers.a = result;
            RESET_SUBTRACT;
            CHECK_AND_SET_ZERO(p->registers.a);
            debug_print("ADC A,B\n", NULL);
			break;
        }
        case 0x89: {
            // ADC A,C
            // 1 4
            // Z 0 H C
            uint8_t result = p->registers.a + p->registers.c + p->flagRegister.carry;
            p->flagRegister.half_carry = is_three_half_carry_add(p->registers.a, p->registers.c, p->flagRegister.carry);
            p->flagRegister.carry      = 0xFF < ((uint16_t) p->registers.a + (uint16_t) p->registers.c + (uint16_t) p->flagRegister.carry);
            p->registers.a = result;
            RESET_SUBTRACT;
            CHECK_AND_SET_ZERO(p->registers.a);
            debug_print("ADC A,C\n", NULL);
			break;
        }
        case 0x8A: {
            // ADC A,D
            // 1 4
            // Z 0 H C
            uint8_t result = p->registers.a + p->registers.d + p->flagRegister.carry;
            p->flagRegister.half_carry = is_three_half_carry_add(p->registers.a, p->registers.d, p->flagRegister.carry);
            p->flagRegister.carry      = 0xFF < ((uint16_t) p->registers.a + (uint16_t) p->registers.d + (uint16_t) p->flagRegister.carry);
            p->registers.a = result;
            RESET_SUBTRACT;
            CHECK_AND_SET_ZERO(p->registers.a);
            debug_print("ADC A,D\n", NULL);
			break;
        }
        case 0x8B: {
            // ADC A,E
            // 1 4
            // Z 0 H C
            uint8_t result = p->registers.a + p->registers.e + p->flagRegister.carry;
            p->flagRegister.half_carry = is_three_half_carry_add(p->registers.a, p->registers.e, p->flagRegister.carry);
            p->flagRegister.carry      = 0xFF < ((uint16_t) p->registers.a + (uint16_t) p->registers.e + (uint16_t) p->flagRegister.carry);
            p->registers.a = result;
            RESET_SUBTRACT;
            CHECK_AND_SET_ZERO(p->registers.a);
            debug_print("ADC A,E\n", NULL);
			break;
        }
        case 0x8C: {
            // ADC A,H
            // 1 4
            // Z 0 H C
            uint8_t result = p->registers.a + p->registers.h + p->flagRegister.carry;
            p->flagRegister.half_carry = is_three_half_carry_add(p->registers.a, p->registers.h, p->flagRegister.carry);
            p->flagRegister.carry      = 0xFF < ((uint16_t) p->registers.a + (uint16_t) p->registers.h + (uint16_t) p->flagRegister.carry);
            p->registers.a = result;
            RESET_SUBTRACT;
            CHECK_AND_SET_ZERO(p->registers.a);
            debug_print("ADC A,H\n", NULL);
			break;
        }
        case 0x8D: {
            // ADC A,L
            // 1 4
            // Z 0 H C
            uint8_t result = p->registers.a + p->registers.l + p->flagRegister.carry;
            p->flagRegister.half_carry = is_three_half_carry_add(p->registers.a, p->registers.l, p->flagRegister.carry);
            p->flagRegister.carry      = 0xFF < ((uint16_t) p->registers.a + (uint16_t) p->registers.l + (uint16_t) p->flagRegister.carry);
            p->registers.a = result;
            RESET_SUBTRACT;
            CHECK_AND_SET_ZERO(p->registers.a);
            debug_print("ADC A,L\n", NULL);
			break;
        }
        case 0x8E: {
            // ADC A,(HL)
            // 1 8
            // Z 0 H C
            uint8_t maccess = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)];
            uint8_t result = p->registers.a + maccess + p->flagRegister.carry;
            p->flagRegister.half_carry = is_three_half_carry_add(p->registers.a, maccess, p->flagRegister.carry);
            p->flagRegister.carry      = 0xFF < ((uint16_t) p->registers.a + (uint16_t) maccess + (uint16_t) p->flagRegister.carry);
            p->registers.a = result;
            RESET_SUBTRACT;
            CHECK_AND_SET_ZERO(p->registers.a);
            debug_print("ADC A,(HL)\n", NULL);
			break;
        }
        case 0x8F: {
            // ADC A,A
            // 1 4
            // Z 0 H C
            uint8_t result = p->registers.a + p->registers.a + p->flagRegister.carry;
            p->flagRegister.half_carry = is_three_half_carry_add(p->registers.a,p->registers.a, p->flagRegister.carry);
            p->flagRegister.carry      = 0xFF < ((uint16_t) p->registers.a + (uint16_t) p->registers.a + (uint16_t) p->flagRegister.carry);
            p->registers.a = result;
            RESET_SUBTRACT;
            CHECK_AND_SET_ZERO(p->registers.a);
            debug_print("ADC A,A\n", NULL);
			break;
        }
        case 0x90: {
            // SUB B
            // 1 4
            // Z 1 H C
            uint16_t result = (uint16_t) p->registers.a - (uint16_t) p->registers.b;
            p->flagRegister.carry = (result & 0xFF00) > 0;
            p->flagRegister.half_carry = is_half_carry_sub(p->registers.a, p->registers.b);
            p->registers.a = result & 0xFF;
            SET_SUBTRACT;
            CHECK_AND_SET_ZERO(p->registers.a);
            debug_print("SUB B\n", NULL);
			break;
        }
        case 0x91: {
            // SUB C
            // 1 4
            // Z 1 H C
            uint16_t result = (uint16_t) p->registers.a - (uint16_t) p->registers.c;
            p->flagRegister.carry = (result & 0xFF00) > 0;
            p->flagRegister.half_carry = is_half_carry_sub(p->registers.a, p->registers.c);
            p->registers.a = result & 0xFF;
            SET_SUBTRACT;
            CHECK_AND_SET_ZERO(p->registers.a);
            debug_print("SUB C\n", NULL);
			break;
        }
        case 0x92: {
            // SUB D
            // 1 4
            // Z 1 H C
            uint16_t result = (uint16_t) p->registers.a - (uint16_t) p->registers.d;
            p->flagRegister.carry = (result & 0xFF00) > 0;
            p->flagRegister.half_carry = is_half_carry_sub(p->registers.a, p->registers.d);
            p->registers.a = result & 0xFF;
            SET_SUBTRACT;
            CHECK_AND_SET_ZERO(p->registers.a);
            debug_print("SUB D\n", NULL);
			break;
        }
        case 0x93: {
            // SUB E
            // 1 4
            // Z 1 H C
            uint16_t result = (uint16_t) p->registers.a - (uint16_t) p->registers.e;
            p->flagRegister.carry = (result & 0xFF00) > 0;
            p->flagRegister.half_carry = is_half_carry_sub(p->registers.a,
                                                           p->registers.e);
            p->registers.a = result & 0xFF;
            SET_SUBTRACT;
            CHECK_AND_SET_ZERO(p->registers.a);
            debug_print("SUB E\n", NULL);
			break;
        }
        case 0x94: {
            // SUB H
            // 1 4
            // Z 1 H C
            uint16_t result = (uint16_t) p->registers.a - (uint16_t) p->registers.h;
            p->flagRegister.carry = (result & 0xFF00) > 0;
            p->flagRegister.half_carry = is_half_carry_sub(p->registers.a, p->registers.h);
            p->registers.a = result & 0xFF;
            SET_SUBTRACT;
            CHECK_AND_SET_ZERO(p->registers.a);
            debug_print("SUB H\n", NULL);
			break;
        }
        case 0x95: {
            // SUB L
            // 1 4
            // Z 1 H C
            uint16_t result = (uint16_t) p->registers.a - (uint16_t) p->registers.l;
            p->flagRegister.carry = (result & 0xFF00) > 0;
            p->flagRegister.half_carry = is_half_carry_sub(p->registers.a, p->registers.l);
            p->registers.a = result & 0xFF;
            SET_SUBTRACT;
            CHECK_AND_SET_ZERO(p->registers.a);
            debug_print("SUB L\n", NULL);
			break;
        }
        case 0x96: {
            // SUB (HL)
            // 1 8
            // Z 1 H C
            uint16_t val = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)];
            uint16_t result = (uint16_t) p->registers.a - (uint16_t) val;
            p->flagRegister.carry = (result & 0xFF00) > 0;
            p->flagRegister.half_carry = is_half_carry_sub(p->registers.a, val);
            p->registers.a = result & 0xFF;
            SET_SUBTRACT;
            CHECK_AND_SET_ZERO(p->registers.a);
            debug_print("SUB (HL)\n", NULL);
			break;
        }
        case 0x97:
            // SUB A
            // 1 4
            // Z 1 H C
            p->registers.a = 0x00;
            p->flagRegister.half_carry = 0;
            p->flagRegister.carry      = 0;
            SET_SUBTRACT;
            CHECK_AND_SET_ZERO(p->registers.a);
            debug_print("SUB A\n", NULL);
			break;
        case 0x98: {
            // SBC A,B
            // 1 4
            // Z 1 H C
            SET_SUBTRACT;
            int16_t result = (uint16_t) p->registers.a - (uint16_t) p->registers.b - (uint16_t) p->flagRegister.carry;
            p->flagRegister.carry = (result & 0xFF00) > 0;
            p->flagRegister.half_carry = is_three_half_carry_sub(p->registers.a, p->registers.b, p->flagRegister.carry);
            p->registers.a = result & 0xFF;
            CHECK_AND_SET_ZERO(p->registers.a);
            debug_print("SBC A,B\n", NULL);
			break;
        }
        case 0x99: {
            // SBC A,C
            // 1 4
            // Z 1 H C
            SET_SUBTRACT;
            int16_t result = (uint16_t) p->registers.a - (uint16_t) p->registers.c - (uint16_t) p->flagRegister.carry;
            p->flagRegister.carry = (result & 0xFF00) > 0;
            p->flagRegister.half_carry = is_three_half_carry_sub(p->registers.a, p->registers.c, p->flagRegister.carry);
            p->registers.a = result & 0xFF;
            CHECK_AND_SET_ZERO(p->registers.a);
            debug_print("SBC A,C\n", NULL);
			break;
        }
        case 0x9A: {
            // SBC A,D
            // 1 4
            // Z 1 H C
            SET_SUBTRACT;
            int16_t result = (uint16_t) p->registers.a - (uint16_t) p->registers.d - (uint16_t) p->flagRegister.carry;
            p->flagRegister.carry = (result & 0xFF00) > 0;
            p->flagRegister.half_carry = is_three_half_carry_sub(p->registers.a, p->registers.d, p->flagRegister.carry);
            p->registers.a = result & 0xFF;
            CHECK_AND_SET_ZERO(p->registers.a);
            debug_print("SBC A,D\n", NULL);
			break;
        }
        case 0x9B: {
            // SBC A,E
            // 1 4
            // Z 1 H C
            SET_SUBTRACT;
            int16_t result = (uint16_t) p->registers.a - (uint16_t) p->registers.e - (uint16_t) p->flagRegister.carry;
            p->flagRegister.carry = (result & 0xFF00) > 0;
            p->flagRegister.half_carry = is_three_half_carry_sub(p->registers.a, p->registers.e, p->flagRegister.carry);
            p->registers.a = result & 0xFF;
            CHECK_AND_SET_ZERO(p->registers.a);
            debug_print("SBC A,E\n", NULL);
			break;
        }
        case 0x9C: {
            // SBC A,H
            // 1 4
            // Z 1 H C
            SET_SUBTRACT;
            int16_t result = (uint16_t) p->registers.a - (uint16_t) p->registers.h - (uint16_t) p->flagRegister.carry;
            p->flagRegister.carry = (result & 0xFF00) > 0;
            p->flagRegister.half_carry = is_three_half_carry_sub(p->registers.a, p->registers.h, p->flagRegister.carry);
            p->registers.a = result & 0xFF;
            CHECK_AND_SET_ZERO(p->registers.a);
            debug_print("SBC A,H\n", NULL);
			break;
        }
        case 0x9D: {
            // SBC A,L
            // 1 4
            // Z 1 H C
            SET_SUBTRACT;
            int16_t result = (uint16_t) p->registers.a - (uint16_t) p->registers.l - (uint16_t) p->flagRegister.carry;
            p->flagRegister.carry = (result & 0xFF00) > 0;
            p->flagRegister.half_carry = is_three_half_carry_sub(p->registers.a, p->registers.l, p->flagRegister.carry);
            p->registers.a = result & 0xFF;
            CHECK_AND_SET_ZERO(p->registers.a);
            debug_print("SBC A,L\n", NULL);
			break;
        }
        case 0x9E: {
            // SBC A,(HL)
            // 1 8
            // Z 1 H C
            SET_SUBTRACT;
            int8_t     val = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)];
            int16_t result = (uint16_t) p->registers.a - (uint16_t) val - (uint16_t) p->flagRegister.carry;
            p->flagRegister.carry = (result & 0xFF00) > 0;
            p->flagRegister.half_carry = is_three_half_carry_sub(p->registers.a, val, p->flagRegister.carry);
            p->registers.a = result & 0xFF;
            CHECK_AND_SET_ZERO(p->registers.a);
            debug_print("SBC A,(HL)\n", NULL);
			break;
        }
        case 0x9F: {
            // SBC A,A
            // 1 4
            // Z 1 H C
            SET_SUBTRACT;
            int16_t result = (uint16_t) p->registers.a - (uint16_t) p->registers.a - (uint16_t) p->flagRegister.carry;
            p->flagRegister.carry = (result & 0xFF00) > 0;
            p->flagRegister.half_carry = is_three_half_carry_sub(p->registers.a, p->registers.a, p->flagRegister.carry);
            p->registers.a = result & 0xFF;
            CHECK_AND_SET_ZERO(p->registers.a);
            debug_print("SBC A,A\n", NULL);
			break;
        }
        case 0xA0:
            // AND B
            // 1 4
            // Z 0 1 0
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            RESET_CARRY;
            p->registers.a = p->registers.a && p->registers.b;
            p->flagRegister.zero = p->registers.a == 0;
            debug_print("AND B\n", NULL);
			break;
        case 0xA1:
            // AND C
            // 1 4
            // Z 0 1 0
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            RESET_CARRY;
            p->registers.a = p->registers.a && p->registers.c;
            p->flagRegister.zero = p->registers.a == 0;
            debug_print("AND C\n", NULL);
			break;
        case 0xA2:
            // AND D
            // 1 4
            // Z 0 1 0
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            RESET_CARRY;
            p->registers.a = p->registers.a && p->registers.d;
            p->flagRegister.zero = p->registers.a == 0;
            debug_print("AND D\n", NULL);
			break;
        case 0xA3:
            // AND E
            // 1 4
            // Z 0 1 0
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            RESET_CARRY;
            p->registers.a = p->registers.a && p->registers.e;
            p->flagRegister.zero = p->registers.a == 0;
            debug_print("AND E\n", NULL);
			break;
        case 0xA4:
            // AND H
            // 1 4
            // Z 0 1 0
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            RESET_CARRY;
            p->registers.a = p->registers.a && p->registers.h;
            p->flagRegister.zero = p->registers.a == 0;
            debug_print("AND H\n", NULL);
			break;
        case 0xA5:
            // AND L
            // 1 4
            // Z 0 1 0
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            RESET_CARRY;
            p->registers.a = p->registers.a && p->registers.l;
            p->flagRegister.zero = p->registers.a == 0;
            debug_print("AND L\n", NULL);
			break;
        case 0xA6:
            // AND (HL)
            // 1 8
            // Z 0 1 0
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            RESET_CARRY;
            p->registers.a = p->registers.a && p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)];
            p->flagRegister.zero = p->registers.a == 0;
            debug_print("AND (HL)\n", NULL);
			break;
        case 0xA7:
            // AND A
            // 1 4
            // Z 0 1 0
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            RESET_CARRY;
            p->registers.a = p->registers.a && p->registers.a;
            p->flagRegister.zero = p->registers.a == 0;
            debug_print("AND A\n", NULL);
			break;
        case 0xA8:
            // XOR B
            // 1 4
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            RESET_CARRY;
            debug_print("XOR B\n", NULL);
			break;
        case 0xA9:
            // XOR C
            // 1 4
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            RESET_CARRY;
            debug_print("XOR C\n", NULL);
			break;
        case 0xAA:
            // XOR D
            // 1 4
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            RESET_CARRY;
            debug_print("XOR D\n", NULL);
			break;
        case 0xAB:
            // XOR E
            // 1 4
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            RESET_CARRY;
            debug_print("XOR E\n", NULL);
			break;
        case 0xAC:
            // XOR H
            // 1 4
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            RESET_CARRY;
            debug_print("XOR H\n", NULL);
			break;
        case 0xAD:
            // XOR L
            // 1 4
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            RESET_CARRY;
            debug_print("XOR L\n", NULL);
			break;
        case 0xAE:
            // XOR (HL)
            // 1 8
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            RESET_CARRY;
            debug_print("XOR (HL)\n", NULL);
			break;
        case 0xAF:
            // XOR A
            // 1 4
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            RESET_CARRY;
            debug_print("XOR A\n", NULL);
			break;
        case 0xB0:
            // OR B
            // 1 4
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            RESET_CARRY;
            debug_print("OR B\n", NULL);
			break;
        case 0xB1:
            // OR C
            // 1 4
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            RESET_CARRY;
            debug_print("OR C\n", NULL);
			break;
        case 0xB2:
            // OR D
            // 1 4
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            RESET_CARRY;
            debug_print("OR D\n", NULL);
			break;
        case 0xB3:
            // OR E
            // 1 4
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            RESET_CARRY;
            debug_print("OR E\n", NULL);
			break;
        case 0xB4:
            // OR H
            // 1 4
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            RESET_CARRY;
            debug_print("OR H\n", NULL);
			break;
        case 0xB5:
            // OR L
            // 1 4
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            RESET_CARRY;
            debug_print("OR L\n", NULL);
			break;
        case 0xB6:
            // OR (HL)
            // 1 8
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            RESET_CARRY;
            debug_print("OR (HL)\n", NULL);
			break;
        case 0xB7:
            // OR A
            // 1 4
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            RESET_CARRY;
            debug_print("OR A\n", NULL);
			break;
        case 0xB8:
            // CP B
            // 1 4
            // Z 1 H C
            p->flagRegister.zero = p->registers.a == p->registers.b;
            SET_SUBTRACT;
            // set if "no borrow"
            p->flagRegister.half_carry = !is_half_carry_sub(p->registers.a, p->registers.b);
            p->flagRegister.carry = p->registers.a < p->registers.b;
            debug_print("CP B\n", NULL);
			break;
        case 0xB9:
            // CP C
            // 1 4
            // Z 1 H C
            SET_SUBTRACT;
            p->flagRegister.zero = p->registers.a == p->registers.c;
            p->flagRegister.half_carry = !is_half_carry_sub(p->registers.a, p->registers.c);
            p->flagRegister.carry = p->registers.a < p->registers.c;
            debug_print("CP C\n", NULL);
			break;
        case 0xBA:
            // CP D
            // 1 4
            // Z 1 H C
            SET_SUBTRACT;
            p->flagRegister.zero = p->registers.a == p->registers.d;
            p->flagRegister.half_carry = !is_half_carry_sub(p->registers.a, p->registers.d);
            p->flagRegister.carry = p->registers.a < p->registers.d;
            debug_print("CP D\n", NULL);
			break;
        case 0xBB:
            // CP E
            // 1 4
            // Z 1 H C
            SET_SUBTRACT;
            p->flagRegister.zero = p->registers.a == p->registers.e;
            p->flagRegister.half_carry = !is_half_carry_sub(p->registers.a, p->registers.e);
            p->flagRegister.carry = p->registers.a < p->registers.e;
            debug_print("CP E\n", NULL);
			break;
        case 0xBC:
            // CP H
            // 1 4
            // Z 1 H C
            SET_SUBTRACT;
            p->flagRegister.zero = p->registers.a == p->registers.h;
            p->flagRegister.half_carry = !is_half_carry_sub(p->registers.a, p->registers.h);
            p->flagRegister.carry = p->registers.a < p->registers.h;
            debug_print("CP H\n", NULL);
			break;
        case 0xBD:
            // CP L
            // 1 4
            // Z 1 H C
            SET_SUBTRACT;
            p->flagRegister.zero = p->registers.a == p->registers.l;
            p->flagRegister.half_carry = !is_half_carry_sub(p->registers.a, p->registers.l);
            p->flagRegister.carry = p->registers.a < p->registers.l;
            debug_print("CP L\n", NULL);
			break;
        case 0xBE: {
            // CP (HL)
            // 1 8
            // Z 1 H C
            SET_SUBTRACT;
            uint8_t val = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)];
            p->flagRegister.zero = p->registers.a == val;
            p->flagRegister.half_carry = !is_half_carry_sub(p->registers.a, val);
            p->flagRegister.carry = p->registers.a <  val;
            debug_print("CP (HL)\n", NULL);
			break;
        }
        case 0xBF:
            // CP A
            // 1 4
            // Z 1 H C
            SET_SUBTRACT;
            p->flagRegister.zero = 1;
            p->flagRegister.half_carry = !is_half_carry_sub(p->registers.a, p->registers.a);
            p->flagRegister.carry = 0;
            debug_print("CP A\n", NULL);
			break;
        case 0xC0:
            // RET NZ
            // 1 20/8
            // - - - -
            debug_print("RET NZ\n", NULL);
            if (p->flagRegister.zero == CLEAR) {
                goto RETURN_CASE;
            }
			break;
        case 0xC1:
            // POP BC
            // 1 12
            // - - - -
            debug_print("POP BC\n", NULL);
            p->registers.c = p->memory[p->sp];
            p->registers.b = p->memory[p->sp - 1];
            p->sp += 2;
			break;
        case 0xC2:
            // JP NZ,a16
            // 3 16/12
            // - - - -
            debug_print("JP NZ,a16\n", NULL);
            // jump to addr n if the zero flag is reset
            if (!p->flagRegister.zero) {
                p->pc = p->memory[p->pc + 1] + (p->memory[p->pc + 2] << 8);
                bytes_ate = 0;
            }
			break;
        case 0xC3:
            // JP a16
            // 3 16
            // - - - -
            debug_print("JP a16\n", NULL);
            // jump to address nn
            p->pc = p->memory[p->pc + 1] + (p->memory[p->pc + 2] << 8);
            bytes_ate = 0;
			break;
        case 0xC4:
            // CALL NZ,a16
            // 3 24/12
            // - - - -
            debug_print("CALL NZ,a16\n", NULL);
			break;
        case 0xC5:
            // PUSH BC
            // 1 16
            // - - - -
            debug_print("PUSH BC\n", NULL);
            p->memory[p->sp] = p->registers.c;
            p->memory[p->sp - 1] = p->registers.b;
            p->sp -= 2;
			break;
        case 0xC6:
            // ADD A,d8
            // 2 8
            // Z 0 H C
            debug_print("ADD A,d8\n", NULL);
			d8 = p->memory[p->pc + 1];
            p->flagRegister.half_carry = is_half_carry_add(p->registers.a, d8);
            p->flagRegister.carry = 0xFF < ((uint16_t) p->registers.a + (uint16_t) d8);

            p->registers.a += d8;
			CHECK_AND_SET_ZERO(p->registers.a)
            RESET_SUBTRACT;
            bytes_ate = 2;
            break;
        case 0xC7:
            // RST 00H
            // 1 16
            // - - - -
            debug_print("RST 00H\n", NULL);
			break;
        case 0xC8:
            // RET Z
            // 1 20/8
            // - - - -
            debug_print("RET Z\n", NULL);
            if (p->flagRegister.zero == SET) {
                goto RETURN_CASE;
            }
			break;
        case 0xC9: {
            //  RET
            // 1 16
            // - - - -
            debug_print("RET\n", NULL);
            // Remember - stack grows DOWNWARD in value, when you pop you go up in value
RETURN_CASE:;
            uint8_t lower_bits = p->memory[p->sp++];
            uint8_t upper_bits = p->memory[p->sp++];
            combined_value = get_16bit_value(upper_bits, lower_bits);
            // Jump to the address specified by the combined_value
            p->pc = combined_value;
			return;
        }
        case 0xCA:
            // JP Z,a16
            // 3 16/12
            // - - - -
            debug_print("JP Z,a16\n", NULL);
            // jump if z flag is set
            if (p->flagRegister.zero) {
                p->pc = p->memory[p->pc + 1] + (p->memory[p->pc + 2] << 8);
                bytes_ate = 0;
            }
			
			break;
        case 0xCB:
            // PREFIX CB
            // 1 4
            // - - - -
            proc_handle_cb_prefix(p);
            bytes_ate = 2;
            debug_print("PREFIX CB\n", NULL);
			break;
        case 0xCC:
            // CALL Z,a16
            // 3 24/12
            // - - - -
            debug_print("CALL Z,a16\n", NULL);
			break;
        case 0xCD:
            // CALL a16
            // 3 24
            // - - - -
            debug_print("CALL a16\n", NULL);
			break;
        case 0xCE:
            // ADC A,d8
            // 2 8
            // Z 0 H C
            RESET_SUBTRACT;
            d8 = p->memory[p->pc + 1];
            bytes_ate = 2;
            debug_print("ADC A,d8\n", NULL);
			break;
        case 0xCF:
            // RST 08H
            // 1 16
            // - - - -
            debug_print("RST 08H\n", NULL);
			break;
        case 0xD0:
            // RET NC
            // 1 20/8
            // - - - -
            debug_print("RET NC\n", NULL);
            if (p->flagRegister.carry == CLEAR) {
                goto RETURN_CASE;
            }
			break;
        case 0xD1:
            // POP DE
            // 1 12
            // - - - -
            debug_print("POP DE\n", NULL);
            p->registers.e = p->memory[p->sp];
            p->registers.d = p->memory[p->sp - 1];
            p->sp += 2;
			break;
        case 0xD2:
            // JP NC,a16
            // 3 16/12
            // - - - -
            debug_print("JP NC,a16\n", NULL);
            
            if (!p->flagRegister.carry) {
                p->pc = p->memory[p->pc + 1] + (p->memory[p->pc + 2] << 8);
                bytes_ate = 0;
            }
			
			break;
        case 0xD3:
            break;
        case 0xD4:
            // CALL NC,a16
            // 3 24/12
            // - - - -
            debug_print("CALL NC,a16\n", NULL);
			break;
        case 0xD5:
            // PUSH DE
            // 1 16
            // - - - -
            debug_print("PUSH DE\n", NULL);
            p->memory[p->sp] = p->registers.e;
            p->memory[p->sp - 1] = p->registers.d;
            p->sp -= 2;
			break;
        case 0xD6: {
            // SUB d8
            // 2 8
            // Z 1 H C
            debug_print("SUB d8\n", NULL);
            d8 = p->memory[p->pc + 1];
            bytes_ate = 2;
            uint16_t result = (uint16_t) p->registers.a - (uint16_t) d8;
            p->flagRegister.carry = (result & 0xFF00) > 0;
            p->flagRegister.half_carry = is_half_carry_sub(p->registers.a, d8);
            p->registers.a = result & 0xFF;
            CHECK_AND_SET_ZERO(p->registers.a);
			break;
        }
        case 0xD7:
            // RST 10H
            // 1 16
            // - - - -
            debug_print("RST 10H\n", NULL);
			break;
        case 0xD8:
            // RET C
            // 1 20/8
            // - - - -
            debug_print("RET C\n", NULL);
            if (p->flagRegister.carry == SET) {
                goto RETURN_CASE;
            }
			break;
        case 0xD9:
            //  RETI
            // 1 16
            // - - - -
            debug_print("RETI\n", NULL);
			break;
        case 0xDA:
            // JP C,a16
            // 3 16/12
            // - - - -
            debug_print("JP C,a16\n", NULL);
		    if (p->flagRegister.carry) {
                p->pc = p->memory[p->pc + 1] + (p->memory[p->pc + 2] << 8);
                bytes_ate = 0;
            }
            break;
        case 0xDB:
            break;
        case 0xDC:
            // CALL C,a16
            // 3 24/12
            // - - - -
            debug_print("CALL C,a16\n", NULL);
			break;
        case 0xDD:
            break;
        case 0xDE:
            // SBC A,d8
            // 2 8
            // Z 1 H C
            SET_SUBTRACT;
            d8 = p->memory[p->pc + 1];
            bytes_ate = 2;
            debug_print("SBC A,d8\n", NULL);
            CHECK_AND_SET_ZERO(p->registers.a);
			break;
        case 0xDF:
            // RST 18H
            // 1 16
            // - - - -
            debug_print("RST 18H\n", NULL);
			break;
        case 0xE0:
            // LDH (a8),A
            // 2 12
            // - - - -
            debug_print("LDH (a8),A\n", NULL);
			p->memory[0xFF00 + p->memory[p->pc + 1]] = p->registers.a;
            bytes_ate = 2;
            break;
        case 0xE1:
            // POP HL
            // 1 12
            // - - - -
            debug_print("POP HL\n", NULL);
            p->registers.l = p->memory[p->sp];
            p->registers.h = p->memory[p->sp - 1];
            p->sp += 2;
			break;
        case 0xE2:
            // LD (C),A
            // 2 8
            // - - - -
            debug_print("LD (C),A\n", NULL);
            // another possible error
            write_byte(p, 0xFF00 + p->registers.c, p->registers.a);
			break;
        case 0xE3:
            break;
        case 0xE4:
            break;
        case 0xE5:
            // PUSH HL
            // 1 16
            // - - - -
            debug_print("PUSH HL\n", NULL);
            write_byte(p, p->sp, p->registers.l);
            write_byte(p, p->sp - 1, p->registers.h);
            p->sp -= 2;
			break;
        case 0xE6:
            // AND d8
            // 2 8
            // Z 0 1 0
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            RESET_CARRY;

            d8 = p->memory[p->pc + 1];
            bytes_ate = 2;
            p->registers.a = p->registers.a && d8;
            p->flagRegister.zero = p->registers.a == 0;
            debug_print("AND d8\n", NULL);
			break;
        case 0xE7:
            // RST 20H
            // 1 16
            // - - - -
            debug_print("RST 20H\n", NULL);
			break;
        case 0xE8:
            // ADD SP,r8
            // 2 16
            // 0 0 H C
            RESET_ZERO;
            RESET_SUBTRACT;
            debug_print("ADD SP,r8\n", NULL);
			break;
        case 0xE9:
            // JP (HL)
            // 1 4
            // - - - -
            debug_print("JP (HL)\n", NULL);
            // todo i think this is right
            p->pc = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)];
            bytes_ate = 0;
			break;
        case 0xEA:
            // LD (a16),A
            // 3 16
            // - - - -
            debug_print("LD (a16),A\n", NULL);
            write_byte(p, (p->memory[p->pc + 2] << 8) + p->memory[p->pc + 1], p->registers.a);
			bytes_ate = 3;
            break;
        case 0xEB:
            break;
        case 0xEC:
            break;
        case 0xED:
            break;
        case 0xEE:
            // XOR d8
            // 2 8
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            RESET_CARRY;

            d8 = p->memory[p->pc + 1];
            bytes_ate = 2;

            CHECK_AND_SET_ZERO(p->registers.a);
            debug_print("XOR d8\n", NULL);
			break;
        case 0xEF:
            // RST 28H
            // 1 16
            // - - - -
            debug_print("RST 28H\n", NULL);
			break;
        case 0xF0:
            // LDH A,(a8)
            // 2 12
            // - - - -
            debug_print("LDH A,(a8)\n", NULL);
            p->registers.a = p->memory[0xFF00 + p->memory[p->pc + 1]];
            bytes_ate = 2;
			break;
        case 0xF1:
            // POP AF
            // 1 12
            // Z N H C
            // TODO not sure what it means to check if zero?
            debug_print("POP AF\n", NULL);
            p->registers.f = p->memory[p->sp];
            p->registers.a = p->memory[p->sp - 1];
            p->sp += 2;
			break;
        case 0xF2:
            // LD A,(C)
            // 2 8
            // - - - -
            // i think the 2 is wrong... idk possible error
            p->registers.a = p->memory[p->registers.c + 0xFF00];
            debug_print("LD A,(C)\n", NULL);
			break;
        case 0xF3:
            //  DI
            // 1 4
            // - - - -
            debug_print("DI\n", NULL);
			break;
        case 0xF4:
            break;
        case 0xF5:
            // PUSH AF
            // 1 16
            // - - - -
            debug_print("PUSH AF\n", NULL);
            write_byte(p, p->sp, p->registers.f);
            write_byte(p, p->sp - 1, p->registers.a);
            p->sp -= 2;
			break;
        case 0xF6:
            // OR d8
            // 2 8
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            RESET_CARRY;

            d8 = p->memory[p->pc + 1];
            bytes_ate = 2;

            CHECK_AND_SET_ZERO(p->registers.a);
            debug_print("OR d8\n", NULL);
			break;
        case 0xF7:
            // RST 30H
            // 1 16
            // - - - -
            debug_print("RST 30H\n", NULL);
			break;
        case 0xF8:
            // LD HL,SP+r8
            // 2 12
            // 0 0 H C
            RESET_ZERO;
            RESET_SUBTRACT;

            debug_print("LD HL,SP+r8\n", NULL);
            int8_t data  = p->memory[p->pc + 1];
            uint16_t val = data + p->sp;

            p->registers.h = (val & 0xFF00) >> 8;
            p->registers.l = val & 0x00FF;
            // https://robdor.com/2016/08/10/gameboy-emulator-half-carry-flag/
            p->flagRegister.half_carry = is_half_carry_add(data, p->sp);
            // ERROR MaYBE SKETCH
            p->flagRegister.carry = (data >= 0) ? (p->sp > val) : (p->sp < val);
            bytes_ate = 2;
			break;
        case 0xF9:
            // LD SP,HL
            // 1 8
            // - - - -
            debug_print("LD SP,HL\n", NULL);
            p->sp = (p->registers.h << 8) + p->registers.l;
			break;
        case 0xFA:
            // LD A,(a16)
            // 3 16
            // - - - -
            p->registers.a = p->memory[p->memory[p->pc + 1] + (p->memory[p->pc + 2] << 8)];
            bytes_ate = 3;
            debug_print("LD A,(a16)\n", NULL);
			break;
        case 0xFB:
            //  EI
            // 1 4
            // - - - -
            debug_print("EI\n", NULL);
			break;
        case 0xFC:
            break;
        case 0xFD:
            break;
        case 0xFE: {
            // CP d8
            // 2 8
            // Z 1 H C
            SET_SUBTRACT;
            d8 = p->memory[p->pc + 1];
            p->flagRegister.zero = p->registers.a == d8;
            p->flagRegister.half_carry = !is_half_carry_sub(p->registers.a, d8);
            p->flagRegister.carry = p->registers.a < d8;
            bytes_ate = 2;
            debug_print("CP d8\n", NULL);
			break;
        }
        case 0xFF:
            // RST 38H
            // 1 16
            // - - - -
            debug_print("RST 38H\n", NULL);
			break;
    }

    p->pc += bytes_ate;
}

// TODO ...
void proc_handle_cb_prefix(Proc *p) {

    switch (p->memory[++p->pc]) {
        case 0x0: {
            // RLC B
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            // grab carry bit
            uint8_t c = p->registers.b >> 7;
            // shift left 1 and drop carry on thurrr
            p->registers.b = (p->registers.b << 1) | c;

            // set extra flags
            p->flagRegister.carry = c;
            p->flagRegister.zero = (p->registers.b == 0);

            debug_print("RLC B\n", NULL);
			break;
        }
        case 0x1: {
            // RLC C
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            // grab carry bit
            uint8_t c = p->registers.c >> 7;
            // shift left 1 and drop carry on thurrr
            p->registers.c = (p->registers.c << 1) | c;

            // set extra flags
            p->flagRegister.carry = c;
            p->flagRegister.zero = (p->registers.c == 0);

            debug_print("RLC C\n", NULL);
			break;
        }
        case 0x2: {
            // RLC D
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            // grab carry bit
            uint8_t c = p->registers.d >> 7;
            // shift left 1 and drop carry on thurrr
            p->registers.d = (p->registers.d << 1) | c;

            // set extra flags
            p->flagRegister.carry = c;
            p->flagRegister.zero = (p->registers.d == 0);

            debug_print("RLC D\n", NULL);
			break;
        }
        case 0x3: {
            // RLC E
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;

            // grab carry bit
            uint8_t c = p->registers.e >> 7;
            // shift left 1 and drop carry on thurrr
            p->registers.e = (p->registers.e << 1) | c;

            // set extra flags
            p->flagRegister.carry = c;
            p->flagRegister.zero = (p->registers.e == 0);

            debug_print("RLC E\n", NULL);
			break;
        }
        case 0x4: {
            // RLC H
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;

            // grab carry bit
            uint8_t c = p->registers.h >> 7;
            // shift left 1 and drop carry on thurrr
            p->registers.h = (p->registers.h << 1) | c;

            // set extra flags
            p->flagRegister.carry = c;
            p->flagRegister.zero = (p->registers.h == 0);

            debug_print("RLC H\n", NULL);
			break;
        }
        case 0x5: {
            // RLC L
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            // grab carry bit
            uint8_t c = p->registers.l >> 7;
            // shift left 1 and drop carry on thurrr
            p->registers.l = (p->registers.l << 1) | c;

            // set extra flags
            p->flagRegister.carry = c;
            p->flagRegister.zero = (p->registers.l == 0);
            debug_print("RLC L\n", NULL);
			break;
        }
        case 0x6: {
            // RLC (HL)
            // 2 16
            // Z 0 0 C
            // grab carry bit
            uint8_t c = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] >> 7;
            // shift left 1 and drop carry on thurrr
            p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] = (p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] << 1) | c;

            // set extra flags
            p->flagRegister.carry = c;
            p->flagRegister.zero = (p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] == 0);
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            debug_print("RLC (HL)\n", NULL);
			break;
        }
        case 0x7: {
            // RLC A
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            // grab carry bit
            uint8_t c = p->registers.a >> 7;
            // shift left 1 and drop carry on thurrr
            p->registers.a = (p->registers.a << 1) | c;

            // set extra flags
            p->flagRegister.carry = c;
            p->flagRegister.zero = (p->registers.a == 0);
            debug_print("RLC A\n", NULL);
			break;
        }
        case 0x8: {
            // RRC B
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            uint8_t c = p->registers.b & 0x01; // right bit
            p->registers.b = (p->registers.b >> 1) | (c << 7);

            p->flagRegister.zero  = (p->registers.b == 0);
            p->flagRegister.carry = c;
            debug_print("RRC B\n", NULL);
			break;
        }
        case 0x9: {
            // RRC C
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            uint8_t c = p->registers.c & 0x01; // right bit
            p->registers.c = (p->registers.c >> 1) | (c << 7);

            p->flagRegister.zero  = (p->registers.c == 0);
            p->flagRegister.carry = c;
            debug_print("RRC C\n", NULL);
			break;
        }
        case 0xA: {
            // RRC D
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            uint8_t c = p->registers.d & 0x01; // right bit
            p->registers.d = (p->registers.d >> 1) | (c << 7);

            p->flagRegister.zero  = (p->registers.d == 0);
            p->flagRegister.carry = c;
            debug_print("RRC D\n", NULL);
			break;
        }
        case 0xB: {
            // RRC E
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            uint8_t c = p->registers.e & 0x01; // right bit
            p->registers.e = (p->registers.e >> 1) | (c << 7);

            p->flagRegister.zero  = (p->registers.e == 0);
            p->flagRegister.carry = c;
            debug_print("RRC E\n", NULL);
			break;
        }
        case 0xC: {
            // RRC H
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            uint8_t c = p->registers.h & 0x01; // right bit
            p->registers.h = (p->registers.h >> 1) | (c << 7);

            p->flagRegister.zero  = (p->registers.h == 0);
            p->flagRegister.carry = c;
            debug_print("RRC H\n", NULL);
			break;
        }
        case 0xD: {
            // RRC L
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            uint8_t c = p->registers.l & 0x01; // right bit
            p->registers.l = (p->registers.l >> 1) | (c << 7);

            p->flagRegister.zero  = (p->registers.l == 0);
            p->flagRegister.carry = c;
            debug_print("RRC L\n", NULL);
			break;
        }
        case 0xE: {
            // RRC (HL)
            // 2 16
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            uint8_t c = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] & 0x01; // right bit
            p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] = (p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] >> 1) | (c << 7);

            p->flagRegister.zero  = (p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] == 0);
            p->flagRegister.carry = c;
            debug_print("RRC (HL)\n", NULL);
			break;
        }
        case 0xF: {
            // RRC A
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            uint8_t c = p->registers.a & 0x01; // right bit
            p->registers.a = (p->registers.a >> 1) | (c << 7);

            p->flagRegister.zero  = (p->registers.a == 0);
            p->flagRegister.carry = c;
            debug_print("RRC A\n", NULL);
			break;
        }
        case 0x10: {
            // RL B
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            // shift to get carry
            uint8_t c = p->registers.b >> 7;
            // left 1 and then move old carry bit into bit 0
            p->registers.b = (p->registers.b << 1) | p->flagRegister.carry;

            p->flagRegister.zero = (p->registers.b == 0);
            p->flagRegister.carry = c;
            debug_print("RL B\n", NULL);
			break;
        }
        case 0x11: {
            // RL C
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            // shift to get carry
            uint8_t c = p->registers.c >> 7;
            // left 1 and then move old carry bit into bit 0
            p->registers.c = (p->registers.c << 1) | p->flagRegister.carry;

            p->flagRegister.zero = (p->registers.c == 0);
            p->flagRegister.carry = c;
            debug_print("RL C\n", NULL);
			break;
        }
        case 0x12: {
            // RL D
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            // shift to get carry
            uint8_t c = p->registers.d >> 7;
            // left 1 and then move old carry bit into bit 0
            p->registers.d = (p->registers.d << 1) | p->flagRegister.carry;

            p->flagRegister.zero = (p->registers.d == 0);
            p->flagRegister.carry = c;
            debug_print("RL D\n", NULL);
			break;
        }
        case 0x13: {
            // RL E
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            // shift to get carry
            uint8_t c = p->registers.e >> 7;
            // left 1 and then move old carry bit into bit 0
            p->registers.e = (p->registers.e << 1) | p->flagRegister.carry;

            p->flagRegister.zero = (p->registers.e == 0);
            p->flagRegister.carry = c;
            debug_print("RL E\n", NULL);
			break;
        }
        case 0x14: {
            // RL H
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            // shift to get carry
            uint8_t c = p->registers.h >> 7;
            // left 1 and then move old carry bit into bit 0
            p->registers.h = (p->registers.h << 1) | p->flagRegister.carry;

            p->flagRegister.zero = (p->registers.h == 0);
            p->flagRegister.carry = c;
            debug_print("RL H\n", NULL);
			break;
        }
        case 0x15: {
            // RL L
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            // shift to get carry
            uint8_t c = p->registers.l >> 7;
            // left 1 and then move old carry bit into bit 0
            p->registers.l = (p->registers.l << 1) | p->flagRegister.carry;

            p->flagRegister.zero = (p->registers.l == 0);
            p->flagRegister.carry = c;
            debug_print("RL L\n", NULL);
			break;
        }
        case 0x16: {
            // RL (HL)
            // 2 16
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            // shift to get carry
            uint8_t c = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] >> 7;
            // left 1 and then move old carry bit into bit 0
            p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] = (p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)]<< 1) | p->flagRegister.carry;

            p->flagRegister.zero = (p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] == 0);
            p->flagRegister.carry = c;
            debug_print("RL (HL)\n", NULL);
			break;
        }
        case 0x17: {
            // RL A
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            // shift to get carry
            uint8_t c = p->registers.a >> 7;
            // left 1 and then move old carry bit into bit 0
            p->registers.a = (p->registers.a << 1) | p->flagRegister.carry;

            p->flagRegister.zero = (p->registers.a == 0);
            p->flagRegister.carry = c;
            debug_print("RL A\n", NULL);
			break;
        }
        case 0x18: {
            // RR B
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            uint8_t      c = p->registers.b & 0x01;
            p->registers.b = (p->registers.b >> 1) | (p->flagRegister.carry << 7);

            p->flagRegister.zero  = (p->registers.b == 0);
            p->flagRegister.carry = c;
            debug_print("RR B\n", NULL);
			break;
        }
        case 0x19: {
            // RR C
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            uint8_t      c = p->registers.c & 0x01;
            p->registers.c = (p->registers.c >> 1) | (p->flagRegister.carry << 7);

            p->flagRegister.zero  = (p->registers.c == 0);
            p->flagRegister.carry = c;
            debug_print("RR C\n", NULL);
			break;
        }
        case 0x1A: {
            // RR D
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            uint8_t      c = p->registers.d & 0x01;
            p->registers.d = (p->registers.d >> 1) | (p->flagRegister.carry << 7);

            p->flagRegister.zero  = (p->registers.d == 0);
            p->flagRegister.carry = c;
            debug_print("RR D\n", NULL);
			break;
        }
        case 0x1B: {
            // RR E
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            uint8_t      c = p->registers.e & 0x01;
            p->registers.e = (p->registers.e >> 1) | (p->flagRegister.carry << 7);

            p->flagRegister.zero  = (p->registers.e == 0);
            p->flagRegister.carry = c;
            debug_print("RR E\n", NULL);
			break;
        }
        case 0x1C: {
            // RR H
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            uint8_t      c = p->registers.h & 0x01;
            p->registers.h = (p->registers.h >> 1) | (p->flagRegister.carry << 7);

            p->flagRegister.zero  = (p->registers.h == 0);
            p->flagRegister.carry = c;
            debug_print("RR H\n", NULL);
			break;
        }
        case 0x1D: {
            // RR L
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            uint8_t      c = p->registers.l & 0x01;
            p->registers.l = (p->registers.l >> 1) | (p->flagRegister.carry << 7);

            p->flagRegister.zero  = (p->registers.l == 0);
            p->flagRegister.carry = c;
            debug_print("RR L\n", NULL);
			break;
        }
        case 0x1E: {
            // RR (HL)
            // 2 16
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            uint8_t      c = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] & 0x01;
            p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] = (p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] >> 1) | (p->flagRegister.carry << 7);

            p->flagRegister.zero  = (p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] == 0);
            p->flagRegister.carry = c;
            debug_print("RR (HL)\n", NULL);
			break;
        }
        case 0x1F: {
            // RR A
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            uint8_t      c = p->registers.a & 0x01;
            p->registers.a = (p->registers.a >> 1) | (p->flagRegister.carry << 7);

            p->flagRegister.zero  = (p->registers.a == 0);
            p->flagRegister.carry = c;
            debug_print("RR A\n", NULL);
			break;
        }
        case 0x20:
            // SLA B
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            p->flagRegister.carry = (p->registers.b >> 7);
            p->registers.b        = (p->registers.b << 1);
            p->flagRegister.zero  = (p->registers.b == 0);
            debug_print("SLA B\n", NULL);
			break;
        case 0x21:
            // SLA C
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            p->flagRegister.carry = (p->registers.c >> 7);
            p->registers.c        = p->registers.c << 1;
            p->flagRegister.zero  = (p->registers.c == 0);
            debug_print("SLA C\n", NULL);
			break;
        case 0x22:
            // SLA D
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            p->flagRegister.carry = (p->registers.d >> 7);
            p->registers.d        = p->registers.d << 1;
            p->flagRegister.zero  = (p->registers.d == 0);
            debug_print("SLA D\n", NULL);
			break;
        case 0x23:
            // SLA E
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            p->flagRegister.carry = (p->registers.e >> 7);
            p->registers.e        = p->registers.e << 1;
            p->flagRegister.zero  = (p->registers.e == 0);
            debug_print("SLA E\n", NULL);
			break;
        case 0x24:
            // SLA H
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            p->flagRegister.carry = (p->registers.h >> 7);
            p->registers.h        = p->registers.h << 1;
            p->flagRegister.zero  = (p->registers.h == 0);
            debug_print("SLA H\n", NULL);
			break;
        case 0x25:
            // SLA L
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            p->flagRegister.carry = (p->registers.l >> 7);
            p->registers.l        = p->registers.l << 1;
            p->flagRegister.zero  = (p->registers.l == 0);
            debug_print("SLA L\n", NULL);
			break;
        case 0x26:
            // SLA (HL)
            // 2 16
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            p->flagRegister.carry = (p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] >> 7);
            p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] << 1;
            p->flagRegister.zero  = (p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)]== 0);
            debug_print("SLA (HL)\n", NULL);
			break;
        case 0x27:
            // SLA A
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            p->flagRegister.carry = (p->registers.a >> 7);
            p->registers.a        = p->registers.a << 1;
            p->flagRegister.zero  = (p->registers.a == 0);
            debug_print("SLA A\n", NULL);
			break;
        case 0x28: {
            // SRA B
            // 2 8
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            uint8_t c   = p->registers.b & 0x01;
            uint8_t msb = p->registers.b & 0x80; // grab 1000 0000
            p->registers.b = (p->registers.b >> 1) | msb;

            p->flagRegister.zero = (p->registers.b == 0);
            p->flagRegister.carry = c;
            debug_print("SRA B\n", NULL);
			break;
        }
        case 0x29: {
            // SRA C
            // 2 8
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            uint8_t c   = p->registers.c & 0x01;
            uint8_t msb = p->registers.c & 0x80; // grab 1000 0000
            p->registers.c = (p->registers.c >> 1) | msb;

            p->flagRegister.zero = (p->registers.c == 0);
            p->flagRegister.carry = c;
            debug_print("SRA C\n", NULL);
			break;
        }
        case 0x2A: {
            // SRA D
            // 2 8
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            uint8_t c   = p->registers.d & 0x01;
            uint8_t msb = p->registers.d & 0x80; // grab 1000 0000
            p->registers.d = (p->registers.d >> 1) | msb;

            p->flagRegister.zero = (p->registers.d == 0);
            p->flagRegister.carry = c;
            debug_print("SRA D\n", NULL);
			break;
        }
        case 0x2B: {
            // SRA E
            // 2 8
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            uint8_t c   = p->registers.e & 0x01;
            uint8_t msb = p->registers.e & 0x80; // grab 1000 0000
            p->registers.e = (p->registers.e >> 1) | msb;

            p->flagRegister.zero = (p->registers.e == 0);
            p->flagRegister.carry = c;
            debug_print("SRA E\n", NULL);
			break;
        }
        case 0x2C: {
            // SRA H
            // 2 8
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            uint8_t c   = p->registers.h & 0x01;
            uint8_t msb = p->registers.h & 0x80; // grab 1000 0000
            p->registers.h = (p->registers.h >> 1) | msb;

            p->flagRegister.zero = (p->registers.h == 0);
            p->flagRegister.carry = c;
            debug_print("SRA H\n", NULL);
			break;
        }
        case 0x2D: {
            // SRA L
            // 2 8
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            uint8_t c   = p->registers.l & 0x01;
            uint8_t msb = p->registers.l & 0x80; // grab 1000 0000
            p->registers.l = (p->registers.l >> 1) | msb;

            p->flagRegister.zero = (p->registers.l == 0);
            p->flagRegister.carry = c;
            debug_print("SRA L\n", NULL);
			break;
        }
        case 0x2E: {
            // SRA (HL)
            // 2 16
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            uint16_t val = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t) p->registers.l];
            uint8_t c    = val & 0x01;
            uint8_t msb  = val & 0x80; // grab 1000 0000
            p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t) p->registers.l] = (val >> 1) | msb;

            p->flagRegister.zero = (p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t) p->registers.l] == 0);
            p->flagRegister.carry = c;
            debug_print("SRA (HL)\n", NULL);
			break;
        }
        case 0x2F: {
            // SRA A
            // 2 8
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            uint8_t c   = p->registers.a & 0x01;
            uint8_t msb = p->registers.a & 0x80; // grab 1000 0000
            p->registers.a = (p->registers.a >> 1) | msb; // idk why fam 

            p->flagRegister.zero = (p->registers.a == 0);
            p->flagRegister.carry = c;
            debug_print("SRA A\n", NULL);
			break;
        }
        case 0x30:
            // SWAP B
            // 2 8
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            RESET_CARRY;
            debug_print("SWAP B\n", NULL);
			break;
        case 0x31:
            // SWAP C
            // 2 8
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            RESET_CARRY;
            debug_print("SWAP C\n", NULL);
			break;
        case 0x32:
            // SWAP D
            // 2 8
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            RESET_CARRY;
            debug_print("SWAP D\n", NULL);
			break;
        case 0x33:
            // SWAP E
            // 2 8
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            RESET_CARRY;
            debug_print("SWAP E\n", NULL);
			break;
        case 0x34:
            // SWAP H
            // 2 8
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            RESET_CARRY;
            debug_print("SWAP H\n", NULL);
			break;
        case 0x35:
            // SWAP L
            // 2 8
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            RESET_CARRY;
            debug_print("SWAP L\n", NULL);
			break;
        case 0x36:
            // SWAP (HL)
            // 2 16
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            RESET_CARRY;
            debug_print("SWAP (HL)\n", NULL);
			break;
        case 0x37:
            // SWAP A
            // 2 8
            // Z 0 0 0
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            RESET_CARRY;
            debug_print("SWAP A\n", NULL);
			break;
        case 0x38:
            // SRL B
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            p->flagRegister.carry = (p->registers.b & 0x01);
            p->registers.b        = (p->registers.b >> 1) & 0x7F; // MSB -> 0
            p->flagRegister.zero  = (p->registers.b == 0);
            debug_print("SRL B\n", NULL);
			break;
        case 0x39:
            // SRL C
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            p->flagRegister.carry = (p->registers.c & 0x01);
            p->registers.c        = (p->registers.c >> 1) & 0x7F; // MSB -> 0
            p->flagRegister.zero  = (p->registers.c == 0);
            debug_print("SRL C\n", NULL);
			break;
        case 0x3A:
            // SRL D
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            p->flagRegister.carry = (p->registers.d & 0x01);
            p->registers.d        = (p->registers.d >> 1) & 0x7F; // MSB -> 0
            p->flagRegister.zero  = (p->registers.d == 0);
            debug_print("SRL D\n", NULL);
			break;
        case 0x3B:
            // SRL E
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            p->flagRegister.carry = (p->registers.e & 0x01);
            p->registers.e        = (p->registers.e >> 1) & 0x7F; // MSB -> 0
            p->flagRegister.zero  = (p->registers.e == 0);
            debug_print("SRL E\n", NULL);
			break;
        case 0x3C:
            // SRL H
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            p->flagRegister.carry = (p->registers.h & 0x01);
            p->registers.h        = (p->registers.h >> 1) & 0x7F; // MSB -> 0
            p->flagRegister.zero  = (p->registers.h == 0);
            debug_print("SRL H\n", NULL);
			break;
        case 0x3D:
            // SRL L
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            p->flagRegister.carry = (p->registers.l & 0x01);
            p->registers.l        = (p->registers.l >> 1) & 0x7F; // MSB -> 0
            p->flagRegister.zero  = (p->registers.l == 0);
            debug_print("SRL L\n", NULL);
			break;
        case 0x3E:
            // SRL (HL)
            // 2 16
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            p->flagRegister.carry = (p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] & 0x01);
            p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] = (p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] >> 1) & 0x7F; // MSB -> 0
            p->flagRegister.zero  = (p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] == 0);
            debug_print("SRL (HL)\n", NULL);
			break;
        case 0x3F: {
            // SRL A
            // 2 8
            // Z 0 0 C
            RESET_SUBTRACT;
            RESET_HALF_CARRY;
            p->flagRegister.carry = (p->registers.a & 0x01);
            p->registers.a        = (p->registers.a >> 1) & 0x7F; // MSB -> 0
            p->flagRegister.zero  = (p->registers.a == 0);
            debug_print("SRL A\n", NULL);
			break;
        }
        case 0x40:
            // BIT 0,B
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;

            debug_print("BIT 0,B\n", NULL);
			break;
        case 0x41:
            // BIT 0,C
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 0,C\n", NULL);
			break;
        case 0x42:
            // BIT 0,D
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 0,D\n", NULL);
			break;
        case 0x43:
            // BIT 0,E
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 0,E\n", NULL);
			break;
        case 0x44:
            // BIT 0,H
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 0,H\n", NULL);
			break;
        case 0x45:
            // BIT 0,L
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 0,L\n", NULL);
			break;
        case 0x46:
            // BIT 0,(HL)
            // 2 16
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 0,(HL)\n", NULL);
			break;
        case 0x47:
            // BIT 0,A
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 0,A\n", NULL);
			break;
        case 0x48:
            // BIT 1,B
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 1,B\n", NULL);
			break;
        case 0x49:
            // BIT 1,C
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 1,C\n", NULL);
			break;
        case 0x4A:
            // BIT 1,D
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 1,D\n", NULL);
			break;
        case 0x4B:
            // BIT 1,E
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 1,E\n", NULL);
			break;
        case 0x4C:
            // BIT 1,H
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 1,H\n", NULL);
			break;
        case 0x4D:
            // BIT 1,L
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 1,L\n", NULL);
			break;
        case 0x4E:
            // BIT 1,(HL)
            // 2 16
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 1,(HL)\n", NULL);
			break;
        case 0x4F:
            // BIT 1,A
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 1,A\n", NULL);
			break;
        case 0x50:
            // BIT 2,B
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 2,B\n", NULL);
			break;
        case 0x51:
            // BIT 2,C
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 2,C\n", NULL);
			break;
        case 0x52:
            // BIT 2,D
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 2,D\n", NULL);
			break;
        case 0x53:
            // BIT 2,E
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 2,E\n", NULL);
			break;
        case 0x54:
            // BIT 2,H
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 2,H\n", NULL);
			break;
        case 0x55:
            // BIT 2,L
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 2,L\n", NULL);
			break;
        case 0x56:
            // BIT 2,(HL)
            // 2 16
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 2,(HL)\n", NULL);
			break;
        case 0x57:
            // BIT 2,A
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 2,A\n", NULL);
			break;
        case 0x58:
            // BIT 3,B
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 3,B\n", NULL);
			break;
        case 0x59:
            // BIT 3,C
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 3,C\n", NULL);
			break;
        case 0x5A:
            // BIT 3,D
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 3,D\n", NULL);
			break;
        case 0x5B:
            // BIT 3,E
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 3,E\n", NULL);
			break;
        case 0x5C:
            // BIT 3,H
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 3,H\n", NULL);
			break;
        case 0x5D:
            // BIT 3,L
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 3,L\n", NULL);
			break;
        case 0x5E:
            // BIT 3,(HL)
            // 2 16
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 3,(HL)\n", NULL);
			break;
        case 0x5F:
            // BIT 3,A
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 3,A\n", NULL);
			break;
        case 0x60:
            // BIT 4,B
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 4,B\n", NULL);
			break;
        case 0x61:
            // BIT 4,C
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 4,C\n", NULL);
			break;
        case 0x62:
            // BIT 4,D
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 4,D\n", NULL);
			break;
        case 0x63:
            // BIT 4,E
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 4,E\n", NULL);
			break;
        case 0x64:
            // BIT 4,H
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 4,H\n", NULL);
			break;
        case 0x65:
            // BIT 4,L
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 4,L\n", NULL);
			break;
        case 0x66:
            // BIT 4,(HL)
            // 2 16
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 4,(HL)\n", NULL);
			break;
        case 0x67:
            // BIT 4,A
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 4,A\n", NULL);
			break;
        case 0x68:
            // BIT 5,B
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 5,B\n", NULL);
			break;
        case 0x69:
            // BIT 5,C
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 5,C\n", NULL);
			break;
        case 0x6A:
            // BIT 5,D
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 5,D\n", NULL);
			break;
        case 0x6B:
            // BIT 5,E
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 5,E\n", NULL);
			break;
        case 0x6C:
            // BIT 5,H
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 5,H\n", NULL);
			break;
        case 0x6D:
            // BIT 5,L
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 5,L\n", NULL);
			break;
        case 0x6E:
            // BIT 5,(HL)
            // 2 16
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 5,(HL)\n", NULL);
			break;
        case 0x6F:
            // BIT 5,A
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 5,A\n", NULL);
			break;
        case 0x70:
            // BIT 6,B
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 6,B\n", NULL);
			break;
        case 0x71:
            // BIT 6,C
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 6,C\n", NULL);
			break;
        case 0x72:
            // BIT 6,D
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 6,D\n", NULL);
			break;
        case 0x73:
            // BIT 6,E
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 6,E\n", NULL);
			break;
        case 0x74:
            // BIT 6,H
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 6,H\n", NULL);
			break;
        case 0x75:
            // BIT 6,L
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 6,L\n", NULL);
			break;
        case 0x76:
            // BIT 6,(HL)
            // 2 16
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 6,(HL)\n", NULL);
			break;
        case 0x77:
            // BIT 6,A
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 6,A\n", NULL);
			break;
        case 0x78:
            // BIT 7,B
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 7,B\n", NULL);
			break;
        case 0x79:
            // BIT 7,C
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 7,C\n", NULL);
			break;
        case 0x7A:
            // BIT 7,D
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 7,D\n", NULL);
			break;
        case 0x7B:
            // BIT 7,E
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 7,E\n", NULL);
			break;
        case 0x7C:
            // BIT 7,H
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 7,H\n", NULL);
			break;
        case 0x7D:
            // BIT 7,L
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 7,L\n", NULL);
			break;
        case 0x7E:
            // BIT 7,(HL)
            // 2 16
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 7,(HL)\n", NULL);
			break;
        case 0x7F:
            // BIT 7,A
            // 2 8
            // Z 0 1 -
            RESET_SUBTRACT;
            SET_HALF_CARRY;
            debug_print("BIT 7,A\n", NULL);
			break;
        case 0x80:
            // RES 0,B
            // 2 8
            // - - - -
            debug_print("RES 0,B\n", NULL);

            // AND the current value in register b with 11111110
            // to reset the 0th bit
            p->registers.b = p->registers.b & 0xFE;
			break;
        case 0x81:
            // RES 0,C
            // 2 8
            // - - - -
            debug_print("RES 0,C\n", NULL);

            p->registers.c = p->registers.c & 0xFE;
			break;
        case 0x82:
            // RES 0,D
            // 2 8
            // - - - -
            debug_print("RES 0,D\n", NULL);

            p->registers.d = p->registers.d & 0xFE;
			break;
        case 0x83:
            // RES 0,E
            // 2 8
            // - - - -
            debug_print("RES 0,E\n", NULL);

            p->registers.e = p->registers.e & 0xFE;
			break;
        case 0x84:
            // RES 0,H
            // 2 8
            // - - - -
            debug_print("RES 0,H\n", NULL);

            p->registers.h = p->registers.h & 0xFE;
			break;
        case 0x85:
            // RES 0,L
            // 2 8
            // - - - -
            debug_print("RES 0,L\n", NULL);

            p->registers.l = p->registers.l & 0xFE;
			break;
        case 0x86:
            // RES 0,(HL)
            // 2 16
            // - - - -
            debug_print("RES 0,(HL)\n", NULL);

            p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] & 0xFE;
			break;
        case 0x87:
            // RES 0,A
            // 2 8
            // - - - -
            debug_print("RES 0,A\n", NULL);

            p->registers.a = p->registers.a & 0xFE;
			break;
        case 0x88:
            // RES 1,B
            // 2 8
            // - - - -
            debug_print("RES 1,B\n", NULL);

            p->registers.b = p->registers.b & 0xFD;
			break;
        case 0x89:
            // RES 1,C
            // 2 8
            // - - - -
            debug_print("RES 1,C\n", NULL);

            p->registers.c = p->registers.c & 0xFD;
			break;
        case 0x8A:
            // RES 1,D
            // 2 8
            // - - - -
            debug_print("RES 1,D\n", NULL);

            p->registers.d = p->registers.d & 0xFD;
			break;
        case 0x8B:
            // RES 1,E
            // 2 8
            // - - - -
            debug_print("RES 1,E\n", NULL);

            p->registers.e = p->registers.e & 0xFD;
			break;
        case 0x8C:
            // RES 1,H
            // 2 8
            // - - - -
            debug_print("RES 1,H\n", NULL);

            p->registers.h = p->registers.h & 0xFD;
			break;
        case 0x8D:
            // RES 1,L
            // 2 8
            // - - - -
            debug_print("RES 1,L\n", NULL);

            p->registers.l = p->registers.l & 0xFD;
			break;
        case 0x8E:
            // RES 1,(HL)
            // 2 16
            // - - - -
            debug_print("RES 1,(HL)\n", NULL);

            p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] & 0xFD;
			break;
        case 0x8F:
            // RES 1,A
            // 2 8
            // - - - -
            debug_print("RES 1,A\n", NULL);

            p->registers.a = p->registers.a & 0xFD;
			break;
        case 0x90:
            // RES 2,B
            // 2 8
            // - - - -
            debug_print("RES 2,B\n", NULL);

            // 11111011
            p->registers.b = p->registers.b & 0xFB;
			break;
        case 0x91:
            // RES 2,C
            // 2 8
            // - - - -
            debug_print("RES 2,C\n", NULL);
            p->registers.c = p->registers.c & 0xFB;
			break;
        case 0x92:
            // RES 2,D
            // 2 8
            // - - - -
            debug_print("RES 2,D\n", NULL);

            p->registers.d = p->registers.d & 0xFB;
			break;
        case 0x93:
            // RES 2,E
            // 2 8
            // - - - -
            debug_print("RES 2,E\n", NULL);

            p->registers.e = p->registers.e & 0xFB;
			break;
        case 0x94:
            // RES 2,H
            // 2 8
            // - - - -
            debug_print("RES 2,H\n", NULL);

            p->registers.h = p->registers.h & 0xFB;
			break;
        case 0x95:
            // RES 2,L
            // 2 8
            // - - - -
            debug_print("RES 2,L\n", NULL);

            p->registers.l = p->registers.l & 0xFB;
			break;
        case 0x96:
            // RES 2,(HL)
            // 2 16
            // - - - -
            debug_print("RES 2,(HL)\n", NULL);

            p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] & 0xFB;
			break;
        case 0x97:
            // RES 2,A
            // 2 8
            // - - - -
            debug_print("RES 2,A\n", NULL);

            p->registers.a = p->registers.a & 0xFB;
			break;
        case 0x98:
            // RES 3,B
            // 2 8
            // - - - -
            debug_print("RES 3,B\n", NULL);

            // 1111 0111
            p->registers.b = p->registers.b & 0xF7;
			break;
        case 0x99:
            // RES 3,C
            // 2 8
            // - - - -
            debug_print("RES 3,C\n", NULL);

            p->registers.c = p->registers.c & 0xF7;
			break;
        case 0x9A:
            // RES 3,D
            // 2 8
            // - - - -
            debug_print("RES 3,D\n", NULL);

            p->registers.d = p->registers.d & 0xF7;
			break;
        case 0x9B:
            // RES 3,E
            // 2 8
            // - - - -
            debug_print("RES 3,E\n", NULL);

            p->registers.e = p->registers.e & 0xF7;
			break;
        case 0x9C:
            // RES 3,H
            // 2 8
            // - - - -
            debug_print("RES 3,H\n", NULL);

            p->registers.h = p->registers.h & 0xF7;
			break;
        case 0x9D:
            // RES 3,L
            // 2 8
            // - - - -
            debug_print("RES 3,L\n", NULL);

            p->registers.l = p->registers.l & 0xF7;
			break;
        case 0x9E:
            // RES 3,(HL)
            // 2 16
            // - - - -
            debug_print("RES 3,(HL)\n", NULL);

            p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] & 0xF7;
			break;
        case 0x9F:
            // RES 3,A
            // 2 8
            // - - - -
            debug_print("RES 3,A\n", NULL);

            p->registers.a = p->registers.a & 0xF7;
			break;
        case 0xA0:
            // RES 4,B
            // 2 8
            // - - - -
            debug_print("RES 4,B\n", NULL);

            // 1110 1111
            p->registers.b = p->registers.b & 0xEF;
			break;
        case 0xA1:
            // RES 4,C
            // 2 8
            // - - - -
            debug_print("RES 4,C\n", NULL);

            p->registers.c = p->registers.c & 0xEF;
			break;
        case 0xA2:
            // RES 4,D
            // 2 8
            // - - - -
            debug_print("RES 4,D\n", NULL);

            p->registers.d = p->registers.d & 0xEF;
			break;
        case 0xA3:
            // RES 4,E
            // 2 8
            // - - - -
            debug_print("RES 4,E\n", NULL);

            p->registers.e = p->registers.e & 0xEF;
			break;
        case 0xA4:
            // RES 4,H
            // 2 8
            // - - - -
            debug_print("RES 4,H\n", NULL);

            p->registers.h = p->registers.h & 0xEF;
			break;
        case 0xA5:
            // RES 4,L
            // 2 8
            // - - - -
            debug_print("RES 4,L\n", NULL);

            p->registers.l = p->registers.l & 0xEF;
			break;
        case 0xA6:
            // RES 4,(HL)
            // 2 16
            // - - - -
            debug_print("RES 4,(HL)\n", NULL);

            p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] & 0xEF;
			break;
        case 0xA7:
            // RES 4,A
            // 2 8
            // - - - -
            debug_print("RES 4,A\n", NULL);

            p->registers.a = p->registers.a & 0xEF;
			break;
        case 0xA8:
            // RES 5,B
            // 2 8
            // - - - -
            debug_print("RES 5,B\n", NULL);

            // 1101 1111
            p->registers.b = p->registers.b & 0xDF;
			break;
        case 0xA9:
            // RES 5,C
            // 2 8
            // - - - -
            debug_print("RES 5,C\n", NULL);

            p->registers.c = p->registers.c & 0xDF;
			break;
        case 0xAA:
            // RES 5,D
            // 2 8
            // - - - -
            debug_print("RES 5,D\n", NULL);

            p->registers.d = p->registers.d & 0xDF;
			break;
        case 0xAB:
            // RES 5,E
            // 2 8
            // - - - -
            debug_print("RES 5,E\n", NULL);

            p->registers.e = p->registers.e & 0xDF;
			break;
        case 0xAC:
            // RES 5,H
            // 2 8
            // - - - -
            debug_print("RES 5,H\n", NULL);

            p->registers.h = p->registers.h & 0xDF;
			break;
        case 0xAD:
            // RES 5,L
            // 2 8
            // - - - -
            debug_print("RES 5,L\n", NULL);

            p->registers.l = p->registers.l & 0xDF;
			break;
        case 0xAE:
            // RES 5,(HL)
            // 2 16
            // - - - -
            debug_print("RES 5,(HL)\n", NULL);

            p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] & 0xDF;
			break;
        case 0xAF:
            // RES 5,A
            // 2 8
            // - - - -
            debug_print("RES 5,A\n", NULL);

            p->registers.a = p->registers.a & 0xDF;
			break;
        case 0xB0:
            // RES 6,B
            // 2 8
            // - - - -
            debug_print("RES 6,B\n", NULL);

            // 1011 1111
            p->registers.b = p->registers.b & 0xBF;
			break;
        case 0xB1:
            // RES 6,C
            // 2 8
            // - - - -
            debug_print("RES 6,C\n", NULL);

            p->registers.c = p->registers.c & 0xBF;
			break;
        case 0xB2:
            // RES 6,D
            // 2 8
            // - - - -
            debug_print("RES 6,D\n", NULL);

            p->registers.d = p->registers.d & 0xBF;
			break;
        case 0xB3:
            // RES 6,E
            // 2 8
            // - - - -
            debug_print("RES 6,E\n", NULL);

            p->registers.e = p->registers.e & 0xBF;
			break;
        case 0xB4:
            // RES 6,H
            // 2 8
            // - - - -
            debug_print("RES 6,H\n", NULL);

            p->registers.h = p->registers.h & 0xBF;
			break;
        case 0xB5:
            // RES 6,L
            // 2 8
            // - - - -
            debug_print("RES 6,L\n", NULL);

            p->registers.l = p->registers.l & 0xBF;
			break;
        case 0xB6:
            // RES 6,(HL)
            // 2 16
            // - - - -
            debug_print("RES 6,(HL)\n", NULL);

            p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] & 0xBF;
			break;
        case 0xB7:
            // RES 6,A
            // 2 8
            // - - - -
            debug_print("RES 6,A\n", NULL);

            p->registers.a = p->registers.a & 0xBF;
			break;
        case 0xB8:
            // RES 7,B
            // 2 8
            // - - - -
            debug_print("RES 7,B\n", NULL);

            // 0111 1111
            p->registers.b = p->registers.b & 0x7F;
			break;
        case 0xB9:
            // RES 7,C
            // 2 8
            // - - - -
            debug_print("RES 7,C\n", NULL);

            p->registers.c = p->registers.c & 0x7F;
			break;
        case 0xBA:
            // RES 7,D
            // 2 8
            // - - - -
            debug_print("RES 7,D\n", NULL);

            p->registers.d = p->registers.d & 0x7F;
			break;
        case 0xBB:
            // RES 7,E
            // 2 8
            // - - - -
            debug_print("RES 7,E\n", NULL);

            p->registers.e = p->registers.e & 0x7F;
			break;
        case 0xBC:
            // RES 7,H
            // 2 8
            // - - - -
            debug_print("RES 7,H\n", NULL);

            p->registers.h = p->registers.h & 0x7F;
			break;
        case 0xBD:
            // RES 7,L
            // 2 8
            // - - - -
            debug_print("RES 7,L\n", NULL);

            p->registers.l = p->registers.l & 0x7F;
			break;
        case 0xBE:
            // RES 7,(HL)
            // 2 16
            // - - - -
            debug_print("RES 7,(HL)\n", NULL);

            p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] & 0x7F;
			break;
        case 0xBF:
            // RES 7,A
            // 2 8
            // - - - -
            debug_print("RES 7,A\n", NULL);

            p->registers.a = p->registers.a & 0x7F;
			break;
        case 0xC0:
            // SET 0,B
            // 2 8
            // - - - -
            debug_print("SET 0,B\n", NULL);
            // OR the current value in register b with 00000001
            // to set the 0th bit
            p->registers.b = p->registers.b | 0x1;
			break;
        case 0xC1:
            // SET 0,C
            // 2 8
            // - - - -
            debug_print("SET 0,C\n", NULL);
            // OR the current value in register b with 00000001
            // to set the 0th bit
            p->registers.c = p->registers.c | 0x1;
			break;
        case 0xC2:
            // SET 0,D
            // 2 8
            // - - - -
            debug_print("SET 0,D\n", NULL);
            // OR the current value in register b with 00000001
            // to set the 0th bit
            p->registers.d = p->registers.d | 0x1;
			break;
        case 0xC3:
            // SET 0,E
            // 2 8
            // - - - -
            debug_print("SET 0,E\n", NULL);
            // OR the current value in register b with 00000001
            // to set the 0th bit
            p->registers.e = p->registers.e | 0x1;
			break;
        case 0xC4:
            // SET 0,H
            // 2 8
            // - - - -
            debug_print("SET 0,H\n", NULL);
            // OR the current value in register b with 00000001
            // to set the 0th bit
            p->registers.h = p->registers.h | 0x1;
			break;
        case 0xC5:
            // SET 0,L
            // 2 8
            // - - - -
            debug_print("SET 0,L\n", NULL);
            // OR the current value in register b with 00000001
            // to set the 0th bit
            p->registers.l = p->registers.l | 0x1;
			break;
        case 0xC6:
            // SET 0,(HL)
            // 2 16
            // - - - -
            debug_print("SET 0,(HL)\n", NULL);
            // OR the current value in register b with 00000001
            // to set the 0th bit
            p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] | 0x1;
			break;
        case 0xC7:
            // SET 0,A
            // 2 8
            // - - - -
            debug_print("SET 0,A\n", NULL);
            // OR the current value in register b with 00000001
            // to set the 0th bit
            p->registers.a = p->registers.a | 0x1;
			break;
        case 0xC8:
            // SET 1,B
            // 2 8
            // - - - -
            debug_print("SET 1,B\n", NULL);
            p->registers.b = p->registers.b | 0x2;
			break;
        case 0xC9:
            // SET 1,C
            // 2 8
            // - - - -
            debug_print("SET 1,C\n", NULL);
            p->registers.c = p->registers.c | 0x2;
			break;
        case 0xCA:
            // SET 1,D
            // 2 8
            // - - - -
            debug_print("SET 1,D\n", NULL);
            p->registers.d = p->registers.d | 0x2;
			break;
        case 0xCB:
            // SET 1,E
            // 2 8
            // - - - -
            debug_print("SET 1,E\n", NULL);
            p->registers.e = p->registers.e | 0x2;
			break;
        case 0xCC:
            // SET 1,H
            // 2 8
            // - - - -
            debug_print("SET 1,H\n", NULL);
            p->registers.h = p->registers.h | 0x2;
			break;
        case 0xCD:
            // SET 1,L
            // 2 8
            // - - - -
            debug_print("SET 1,L\n", NULL);
            p->registers.l = p->registers.l | 0x2;
			break;
        case 0xCE:
            // SET 1,(HL)
            // 2 16
            // - - - -
            debug_print("SET 1,(HL)\n", NULL);
            p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] | 0x2;
			break;
        case 0xCF:
            // SET 1,A
            // 2 8
            // - - - -
            debug_print("SET 1,A\n", NULL);
            p->registers.a = p->registers.a | 0x2;
			break;
        case 0xD0:
            // SET 2,B
            // 2 8
            // - - - -
            debug_print("SET 2,B\n", NULL);
            p->registers.b = p->registers.b | 0x4;
			break;
        case 0xD1:
            // SET 2,C
            // 2 8
            // - - - -
            debug_print("SET 2,C\n", NULL);
            p->registers.c = p->registers.c | 0x4;
			break;
        case 0xD2:
            // SET 2,D
            // 2 8
            // - - - -
            debug_print("SET 2,D\n", NULL);
            p->registers.d = p->registers.d | 0x4;
			break;
        case 0xD3:
            // SET 2,E
            // 2 8
            // - - - -
            debug_print("SET 2,E\n", NULL);
            p->registers.e = p->registers.e | 0x4;
			break;
        case 0xD4:
            // SET 2,H
            // 2 8
            // - - - -
            debug_print("SET 2,H\n", NULL);
            p->registers.h = p->registers.h | 0x4;
			break;
        case 0xD5:
            // SET 2,L
            // 2 8
            // - - - -
            debug_print("SET 2,L\n", NULL);
            p->registers.l = p->registers.l | 0x4;
			break;
        case 0xD6:
            // SET 2,(HL)
            // 2 16
            // - - - -
            debug_print("SET 2,(HL)\n", NULL);
            p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] | 0x4;
			break;
        case 0xD7:
            // SET 2,A
            // 2 8
            // - - - -
            debug_print("SET 2,A\n", NULL);
            p->registers.a = p->registers.a | 0x4;
			break;
        case 0xD8:
            // SET 3,B
            // 2 8
            // - - - -
            debug_print("SET 3,B\n", NULL);
            p->registers.b = p->registers.b | 0x8;
			break;
        case 0xD9:
            // SET 3,C
            // 2 8
            // - - - -
            debug_print("SET 3,C\n", NULL);
            p->registers.c = p->registers.c | 0x8;
			break;
        case 0xDA:
            // SET 3,D
            // 2 8
            // - - - -
            debug_print("SET 3,D\n", NULL);
            p->registers.d = p->registers.d | 0x8;
			break;
        case 0xDB:
            // SET 3,E
            // 2 8
            // - - - -
            debug_print("SET 3,E\n", NULL);
            p->registers.e = p->registers.e | 0x8;
			break;
        case 0xDC:
            // SET 3,H
            // 2 8
            // - - - -
            debug_print("SET 3,H\n", NULL);
            p->registers.h = p->registers.h | 0x8;
			break;
        case 0xDD:
            // SET 3,L
            // 2 8
            // - - - -
            debug_print("SET 3,L\n", NULL);
            p->registers.l = p->registers.l | 0x8;
			break;
        case 0xDE:
            // SET 3,(HL)
            // 2 16
            // - - - -
            debug_print("SET 3,(HL)\n", NULL);
            p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] | 0x8;
			break;
        case 0xDF:
            // SET 3,A
            // 2 8
            // - - - -
            debug_print("SET 3,A\n", NULL);
            p->registers.a = p->registers.a | 0x8;
			break;
        case 0xE0:
            // SET 4,B
            // 2 8
            // - - - -
            debug_print("SET 4,B\n", NULL);
            p->registers.b = p->registers.b | 0x10;
			break;
        case 0xE1:
            // SET 4,C
            // 2 8
            // - - - -
            debug_print("SET 4,C\n", NULL);
            p->registers.c = p->registers.c | 0x10;
			break;
        case 0xE2:
            // SET 4,D
            // 2 8
            // - - - -
            debug_print("SET 4,D\n", NULL);
            p->registers.d = p->registers.d | 0x10;
			break;
        case 0xE3:
            // SET 4,E
            // 2 8
            // - - - -
            debug_print("SET 4,E\n", NULL);
            p->registers.e = p->registers.e | 0x10;
			break;
        case 0xE4:
            // SET 4,H
            // 2 8
            // - - - -
            debug_print("SET 4,H\n", NULL);
            p->registers.h = p->registers.h | 0x10;
			break;
        case 0xE5:
            // SET 4,L
            // 2 8
            // - - - -
            debug_print("SET 4,L\n", NULL);
            p->registers.l = p->registers.l | 0x10;
			break;
        case 0xE6:
            // SET 4,(HL)
            // 2 16
            // - - - -
            debug_print("SET 4,(HL)\n", NULL);
            p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] | 0x10;
			break;
        case 0xE7:
            // SET 4,A
            // 2 8
            // - - - -
            debug_print("SET 4,A\n", NULL);
            p->registers.a = p->registers.a | 0x10;
			break;
        case 0xE8:
            // SET 5,B
            // 2 8
            // - - - -
            debug_print("SET 5,B\n", NULL);
            p->registers.b = p->registers.b | 0x20;
			break;
        case 0xE9:
            // SET 5,C
            // 2 8
            // - - - -
            debug_print("SET 5,C\n", NULL);
            p->registers.c = p->registers.c | 0x20;
			break;
        case 0xEA:
            // SET 5,D
            // 2 8
            // - - - -
            debug_print("SET 5,D\n", NULL);
            p->registers.d = p->registers.d | 0x20;
			break;
        case 0xEB:
            // SET 5,E
            // 2 8
            // - - - -
            debug_print("SET 5,E\n", NULL);
            p->registers.e = p->registers.e | 0x20;
			break;
        case 0xEC:
            // SET 5,H
            // 2 8
            // - - - -
            debug_print("SET 5,H\n", NULL);
            p->registers.h = p->registers.h | 0x20;
			break;
        case 0xED:
            // SET 5,L
            // 2 8
            // - - - -
            debug_print("SET 5,L\n", NULL);
            p->registers.l = p->registers.l | 0x20;
			break;
        case 0xEE:
            // SET 5,(HL)
            // 2 16
            // - - - -
            debug_print("SET 5,(HL)\n", NULL);
            p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] | 0x20;
			break;
        case 0xEF:
            // SET 5,A
            // 2 8
            // - - - -
            debug_print("SET 5,A\n", NULL);
            p->registers.a = p->registers.a | 0x20;
			break;
        case 0xF0:
            // SET 6,B
            // 2 8
            // - - - -
            debug_print("SET 6,B\n", NULL);
            p->registers.b = p->registers.b | 0x40;
			break;
        case 0xF1:
            // SET 6,C
            // 2 8
            // - - - -
            debug_print("SET 6,C\n", NULL);
            p->registers.c = p->registers.c | 0x40;
			break;
        case 0xF2:
            // SET 6,D
            // 2 8
            // - - - -
            debug_print("SET 6,D\n", NULL);
            p->registers.d = p->registers.d | 0x40;
			break;
        case 0xF3:
            // SET 6,E
            // 2 8
            // - - - -
            debug_print("SET 6,E\n", NULL);
            p->registers.e = p->registers.e | 0x40;
			break;
        case 0xF4:
            // SET 6,H
            // 2 8
            // - - - -
            debug_print("SET 6,H\n", NULL);
            p->registers.h = p->registers.h | 0x40;
			break;
        case 0xF5:
            // SET 6,L
            // 2 8
            // - - - -
            debug_print("SET 6,L\n", NULL);
            p->registers.l = p->registers.l | 0x40;
			break;
        case 0xF6:
            // SET 6,(HL)
            // 2 16
            // - - - -
            debug_print("SET 6,(HL)\n", NULL);
            p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] | 0x40;
			break;
        case 0xF7:
            // SET 6,A
            // 2 8
            // - - - -
            debug_print("SET 6,A\n", NULL);
            p->registers.a = p->registers.a | 0x40;
			break;
        case 0xF8:
            // SET 7,B
            // 2 8
            // - - - -
            debug_print("SET 7,B\n", NULL);
            p->registers.b = p->registers.b | 0x80;
			break;
        case 0xF9:
            // SET 7,C
            // 2 8
            // - - - -
            debug_print("SET 7,C\n", NULL);
            p->registers.c = p->registers.c | 0x80;
			break;
        case 0xFA:
            // SET 7,D
            // 2 8
            // - - - -
            debug_print("SET 7,D\n", NULL);
            p->registers.d = p->registers.d | 0x80;
			break;
        case 0xFB:
            // SET 7,E
            // 2 8
            // - - - -
            debug_print("SET 7,E\n", NULL);
            p->registers.e = p->registers.e | 0x80;
			break;
        case 0xFC:
            // SET 7,H
            // 2 8
            // - - - -
            debug_print("SET 7,H\n", NULL);
            p->registers.h = p->registers.h | 0x80;
			break;
        case 0xFD:
            // SET 7,L
            // 2 8
            // - - - -
            debug_print("SET 7,L\n", NULL);
            p->registers.l = p->registers.l | 0x80;
			break;
        case 0xFE:
            // SET 7,(HL)
            // 2 16
            // - - - -
            debug_print("SET 7,(HL)\n", NULL);
            p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] = p->memory[(uint16_t)(p->registers.h << 8) + (uint16_t)(p->registers.l)] | 0x80;
			break;
        case 0xFF:
            // SET 7,A
            // 2 8
            // - - - -
            debug_print("SET 7,A\n", NULL);
            p->registers.a = p->registers.a | 0x80;
			break;
    }

}

void proc_dec_hl(Proc* p) {
    if (p->registers.l == 0) {
        p->registers.h--;
        p->registers.l--; // should overflow
    } else {
        p->registers.l--;
    }
}

void proc_inc_hl(Proc* p) {
    if (p->registers.l == 0xFF) {
        p->registers.h++;
        p->registers.l++;
    } else {
        p->registers.l++;
    }
}
