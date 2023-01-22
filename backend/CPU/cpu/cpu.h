#ifndef CPU_H
#define CPU_H

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "..\buffer.h"
#include "stack\stack.h"
#include "..\cmd.h"
#include "..\asm\assembler.h"

static const int  DEFAULT_CAPACITY =   50;
static const int REGISTER_CAPACITY =    8;
static const int      RAM_CAPACITY = 1024;

struct cpu_t {
        int registers[REGISTER_CAPACITY] = {};
        int RAM[RAM_CAPACITY] = {};
};

int divide_cmds (code_t *code);
int execute_code (code_t *code, stack *stk, cpu_t *cpu);
int tens_count (int num);
int source_file_ctor (char *input_file_name, char *argv);

#endif /*CPU_H*/
