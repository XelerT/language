#ifndef CPU_H
#define CPU_H

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "..\buffer.h"
#include "..\cmd.h"

int divide_cmds (code_t *code);
int dis_asm_code (code_t *code, const char *input_file_name);
void append_txt (char *output_file_name);
int source_file_ctor (FILE *source_code, char *input_file_name, char *argv);
void make_output_file_name(char *output_file_name, const char *input_file_name);

#endif /*CPU_H*/
