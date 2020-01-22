#include "helpers.h"

#include <stdio.h>

#define print(s) printf("\x1B[32m"); printf(s); printf("\x1B[0m\n");
#define incorrect(s) printf("\x1B[31m"); printf(s); printf("\x1B[0m\n"); RET_STATUS = 1;

int main() {
    int RET_STATUS = 0;

    uint8_t first_number = (uint8_t) 3;     // 0x0011
    uint8_t second_number = (uint8_t) 12;   // 0x1100

    print("testing no half carry addition, 0x0011 + 0x1100");
    if (is_half_carry_add(first_number, second_number)) {
        incorrect("\tincorrectly got half carry");
    } else {
        print("\tcorrect");
    }

    first_number = (uint8_t) 7;     // 0x0111
    second_number = (uint8_t) 12;   // 0x1100

    print("testing no half carry addition, 0x0111 + 0x1100");
    if (is_half_carry_add(first_number, second_number)) {
        print("\tcorrectly got half carry");
    } else {
        incorrect("\tincorrect");
    }

    first_number = (uint8_t) 7;     // 0x0111
    second_number = (uint8_t) 12;   // 0x1100

    print("testing half carry subtraction, 0x0111 - 0x1100");
    if (is_half_carry_sub(first_number, second_number)) {
        print("\tcorrectly got half carry");
    } else {
        incorrect("\tincorrect");
    }

    first_number = (uint8_t) 12;     // 0x1100
    second_number = (uint8_t) 7;   // 0x0111

    print("testing no half carry subtraction, 0x1100 - 0x1110");
    if (is_half_carry_sub(first_number, second_number)) {
        incorrect("\tincorrectly got half carry");
    } else {
        print("\tcorrect");
    }
}