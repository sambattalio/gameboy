// main.c

#include <stdio.h>
#include "proc.h"

int main(int argc, char **argv) {

    proc* processor = proc_create();

    proc_read_word(processor);

    return 0; 
}