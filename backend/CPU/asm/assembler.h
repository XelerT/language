#ifndef ASM_H
#define ASM_H
#include "..\buffer.h"
#include "..\cmd.h"

const int MAX_N_LABELS = 50;
const int MAX_LINE_LENGTH = 100;

struct labels_t
{
        int line = 0;
        char name[MAX_LINE_LENGTH] = {};
};

int convert_code (code_t *code, FILE *output_code, int second, labels_t *labels, int *asm_code);
int check_argv (int argc, char *argv[]);
void append_txt (char *output_file_name);
int create_label (labels_t *labels, const char *cmd, int n_line, int is_call);
int get_jmp_line (labels_t *labels, char *name);
int get_push_code (const char *val, int *asm_code);
int find_reg (const char *val);
int get_pp_code (const char *val, int *asm_code, const char *name, int coeff);
char* get_free_label_name (labels_t *label);
void listing (code_t *code, int *asm_code, char *function, int line);
int asm_jmp_call (int second_cycle, code_t *code, int *asm_code, char *cmd, char *name, labels_t *labels, size_t i, int *ip);
void free_buf (char *buf);

int pre_asm (code_t *code);
int paste_new_part (line_t *lines, char *part1, char *part2, char *buf,
                    size_t *char_counter, size_t *lines_counter, int is_pop, char op);

size_t count_op (code_t *code);
char contain_op (char *str, size_t length);
char contain_symb (char *str, size_t length, char symb);

#endif /*ASM_H*/
