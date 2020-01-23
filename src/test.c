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

    first_number = (uint8_t) 120; // 0x78
    second_number = (uint8_t) 20; // 0x14;

    print("testing two 8 bit -> 16 bit conversion")
    // should be 0x7814
    uint16_t combined_value = get_16bit_value(first_number, second_number);
    if (combined_value != 30740) {
        incorrect("\twrong number from get 16 bit value");
        printf("\t%d\n", combined_value);
    } else {
        print("\tcorrect");
        printf("\t%d\n", combined_value);
    }

    print("testing splitting 16 bit value into lower 8 bits")
    uint8_t lower = get_lower_8bit_value(combined_value);
    if (lower != 20) {
        incorrect("\tincorrect")
    } else {
        print("\tcorrect");
    }
    printf("\t%d\n", lower);

    print("testing splitting 16 bit value into upper 8 bits")
    uint8_t upper = get_upper_8bit_value(combined_value);
    if (upper != 120) {
        incorrect("\tincorrect")
    } else {
        print("\tcorrect");
    }
    printf("\t%d\n", upper);

    return RET_STATUS;
}