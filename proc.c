// proc.c
#include "proc.h"


proc* proc_create() {
    proc *p = calloc(1, sizeof(proc));
    if (p) {
        p->pc = 0;
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

    // get word....

}