#ifndef PROC_H
#define PROC_H

typedef struct {
    unsigned pc;
} proc;

proc* proc_create();
void  proc_delete(proc* p);

void proc_handle_word(proc *p);

#endif