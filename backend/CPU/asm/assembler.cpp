#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "assembler.h"
#include "../buffer.h"

int check_argv (int argc, char **argv)
{
        if (argc != 3)
                printf ("You need to enter 3 arguments.\n");
        if (argv[2] == 0)
                printf("Enter name of the file with source code.\n");

        if (argv[1] == 0) {
                printf("You need to enter name of the output file.\n");
                return NO_OUTPUT_NAME;
        }

        return 0;
}

int pre_asm (code_t *code)
{
        assert(code);

        int n_new_lines  = 0;
        size_t n_oper = count_op(code);
        if (!n_oper)
                return 0;
        char *buf = (char*) calloc(code->n_chars + n_oper * 50 + 1, sizeof(char));
        if (!buf) {
                fprintf(stderr, "Calloc returned NULL.\n");
                return NULL_CALLOC;
        }
        line_t *lines = (line_t*) calloc(code->n_lines + n_oper * 5 + 1, sizeof(line_t));
        if (!lines) {
                fprintf(stderr, "Calloc returned NULL.\n");
                return NULL_CALLOC;
        }
        size_t char_counter = 0;
        size_t lines_counter = 0;
        size_t temp = 0;
        char part1[MAX_LINE_LENGTH] = {'\0'};
        char part2[MAX_LINE_LENGTH] = {'\0'};
        char op = 0;

        for (size_t i = 0; i < code->n_lines; i++) {
                op = contain_op(code->lines[i].ptr, code->lines[i].length);
                if (op && contain_symb(code->lines[i].ptr, code->lines[i].length, '[')) {
                        int is_pop  = !strncmp(code->lines[i].ptr, "pop [", 5);
                        int is_push = !strncmp(code->lines[i].ptr, "push [", 6);
                        if (is_pop == is_push) {
                                is_pop = !strncmp(code->lines[i].ptr, "popf [", 6);
                                is_push = !strncmp(code->lines[i].ptr, "pushf [", 7);
                                if (is_pop == is_push) {
                                        fprintf(stderr, "Lexical mistake in %ld line. \"%s\"\n", i, code->lines[i].ptr);
                                        return LEX_ERROR;
                                }
                        }
                        if (is_pop)
                                temp = 5;
                        else
                                temp = 6;

                        int n_chars = 0;
                        sscanf(code->lines[i].ptr + temp, "%s %n", part1, &n_chars);
                        temp += (size_t) n_chars + 1;
                        sscanf(code->lines[i].ptr + temp, "%[^]] %n", part2, &n_chars);

                        paste_new_part(lines, part1, part2, buf, &char_counter, &lines_counter, is_pop, op);
                        n_new_lines += 4;
                } else {
                        temp = sprintf(buf + char_counter, "%s", code->lines[i].ptr) + 1;
                        lines[lines_counter].ptr = buf + char_counter;
                        lines[lines_counter].length = temp;
                        lines_counter++;
                        char_counter += temp;
                }
        }
        code->n_lines += n_new_lines;
        code->n_chars += char_counter;

        free(code->lines);
        free(code->buf);

        code->lines = lines;
        code->buf   =   buf;

        return n_new_lines;
}

#define next_line       lines[*lines_counter].ptr = buf + *char_counter;          \
                        lines[*lines_counter].length = temp;                      \
                        ++*lines_counter;                                         \
                        *char_counter += temp;

int paste_new_part (line_t *lines, char *part1, char *part2, char *buf,
                    size_t *char_counter, size_t *lines_counter, int is_pop, char op)
{
        assert(part1);
        assert(part2);
        assert(buf);
        assert(char_counter);
        assert(lines_counter);

        size_t temp = sprintf(buf + *char_counter, "push %s", part1) + 1;
        next_line
        temp = sprintf(buf + *char_counter, "push %s", part2) + 1;
        next_line
        switch (op) {
        case '+':
                temp = sprintf(buf + *char_counter, "add") + 1;
                break;
        case '-':
                temp = sprintf(buf + *char_counter, "sub") + 1;
                break;
        case '*':
                temp = sprintf(buf + *char_counter, "mul") + 1;
                break;
        default:
                return LEX_ERROR;
        }
        next_line
        temp += sprintf(buf + *char_counter, "pop rcx") + 1;
        next_line
        if (is_pop)
                temp = sprintf(buf + *char_counter, "pop [rcx]") + 1;
        else
                temp = sprintf(buf + *char_counter, "push [rcx]") + 1;
        next_line

        return 0;
}
#undef next_line

size_t count_op (code_t *code)
{
        assert(code);

        size_t count = 0;
        for (size_t i = 0; i < code->n_chars; i++) {
                if (code->buf[i] == '+' || code->buf[i] == '-' || code->buf[i] == '*')
                        count++;
        }

        return count;
}

char contain_symb (char *str, size_t length, char symb)
{
        assert(str);

        for (size_t i = length - 1; i; i--) {
                if (str[i] == symb) {
                        return str[i];
                }
        }

        return 0;
}

char contain_op (char *str, size_t length)
{
        assert(str);

        char symb = contain_symb(str, length, '*');
        if (symb)
                return symb;

        symb = contain_symb(str, length, '+');
        if (symb)
                return symb;

        symb = contain_symb(str, length, '-');

        return symb;
}

int convert_code (code_t *code, FILE *output_code, int second_cycle, labels_t *labels, int *asm_code)
{
        assert(code);
        assert(output_code);

        char cmd[MAX_NAME_LENGTH]  = {};
        char name[MAX_NAME_LENGTH] = {};
        int ip = 0;

        for (size_t i = 0; i < code->n_lines; i++) {
                sscanf(code->lines[i].ptr, "%s", cmd);

#include "asm_instructions.en"

                /*else*/ if (strrchr(cmd, ':')) {
                        if (!second_cycle)
                                create_label(labels, cmd, ip, 0);
                        asm_code[ip++] = CMD_LABEL;
                }
                free_buf(cmd);
        }
        if (second_cycle) {
                return ip;
        } else {
                if (convert_code(code, output_code, 1, labels, asm_code) == NO_LABEL)
                        return NO_LABEL;
        }
        for (int i = 0; i < ip; i++) {
                fprintf(output_code, "%d ", asm_code[i]);
        }

        // listing (code, asm_code, "fuck", 69);

        return 0;
}

int asm_jmp_call (int second_cycle, code_t *code, int *asm_code,
             char *cmd, char *name, labels_t *labels, size_t i, int *ip)
{
        assert(code);
        assert(asm_code);
        assert(cmd);
        assert(name);
        assert(labels);
        assert(ip);


        if (second_cycle) {
                sscanf(code->lines[i].ptr + strlen(cmd), "%s", name);
                if ((asm_code[*ip] = get_jmp_line(labels, name)) == NO_LABEL)
                        return NO_LABEL;
                // printf("asm line %d\n", asm_code[*ip - 1]);
        } else {
                char label_name[MAX_NAME_LENGTH] = {0};
                sscanf(code->lines[i].ptr + strlen(cmd), "%s", label_name);
                        if (strcasecmp(cmd, "jmp") == 0)
                                asm_code[*ip] = -1;
                        else if (strcasecmp(cmd, "call") == 0)
                                asm_code[*ip] = -2;
        }
        ++*ip;

        return 0;
}

int get_pp_code (const char *val, int *asm_code, const char *cmd, int coeff)
{
        assert(val);
        assert(asm_code);
        assert(cmd);

        int num = 0;
        int reg = 0;
        char temp_val[MAX_NAME_LENGTH] = {};
        float dot_num = 0;

        if (sscanf(val, "[%s]", temp_val)) {
                num = ARG_RAM;
                if ((reg = find_reg(temp_val)) > 0) {
                        num |= ARG_REG;
                        *asm_code = reg;
                } else if (sscanf(temp_val, "%d", asm_code)) {
                        num |= ARG_IMMED;
                }
        } else if (sscanf(val, "%s", temp_val)) {
                if ((reg = find_reg(temp_val)) > 0) {
                        num = ARG_REG;
                        *asm_code = reg;
                } else if ((strcasecmp(cmd, "push") == 0) || (strcasecmp(cmd, "pushf") == 0)) {
                        if (sscanf(val, "%f", &dot_num)) {
                        // printf("dotnum %f\n", dot_num);
                                *asm_code = (int) (dot_num * (float) coeff);
                                num |= ARG_IMMED;
                        } else if (sscanf(val, "%d", asm_code)) {
                                num = ARG_IMMED;
                        }
                }
        }

        if (strcasecmp(cmd, "pop") == 0  || (strcasecmp(cmd, "popf") == 0))
                return num | CMD_POP;
        else if (strcasecmp(cmd, "push") == 0 || (strcasecmp(cmd, "pushf") == 0))
                return num | CMD_PUSH;

        return 0;
}

void listing (code_t *code, int *asm_code, char *function, int line)
{
        assert(code);
        assert(asm_code);
        assert(function);

        printf("\n\nListing for %s on the %d line.\n\n", function, line);
        int ip = 0;
        for (size_t i = 0; i < code->n_lines; i++, ip++) {
                if (*code->lines[i].ptr != '\0' && *code->lines[i].ptr != '\n')
                        switch (asm_code[ip] & MASK_CMD) {
                        case CMD_PUSH:
                                printf("%d %d %s\t | %d %d\n", ip, ip + 1, code->lines[i].ptr, asm_code[ip], asm_code[ip + 1]);
                                ip++;
                                break;
                        case CMD_PUSHF:
                                printf("%d %d %s\t | %d %d\n", ip, ip + 1, code->lines[i].ptr, asm_code[ip], asm_code[ip + 1]);
                                ip++;
                                break;
                        case CMD_POPF:
                                if ((asm_code[ip] & ARG_REG) || (asm_code[ip] & ARG_RAM))
                                        printf("%d %d | %s\t | %d %d\n", ip, ip + 1, code->lines[i].ptr, asm_code[ip], asm_code[ip + 1]);
                                if (asm_code[ip] & ARG_IMMED)
                                        printf("%d %s\t | %d\n", ip, code->lines[i].ptr, asm_code[ip]);
                                ip++;
                                break;
                        case CMD_POP:
                                if ((asm_code[ip] & ARG_REG) || (asm_code[ip] & ARG_RAM))
                                        printf("%d %d %s\t | %d %d\n", ip, ip + 1, code->lines[i].ptr, asm_code[ip], asm_code[ip + 1]);
                                if (asm_code[ip] & ARG_IMMED)
                                        printf("%d %d %s\t | %d\n", ip, ip + 1, code->lines[i].ptr, asm_code[ip]);
                                ip++;
                                break;
                        case CMD_JA:
                        case CMD_JB:
                        case CMD_JE:
                        case CMD_JMP:
                                printf("%d %d %s\t | %d %d\n", ip, ip + 1, code->lines[i].ptr, asm_code[ip], asm_code[ip + 1]);
                                ip++;
                                break;
                        case CMD_CALL:
                                printf("%d %d %s\t | %d %d\n", ip, ip + 1, code->lines[i].ptr, asm_code[ip], asm_code[ip + 1]);
                                ip++;
                                break;
                        default:
                                printf("%d *%s\t | %d\n", ip, code->lines[i].ptr, asm_code[ip]);
                                break;
                        }
                else
                        ip--;
        }
}

#define DEF_REG(name,...) #name,
int find_reg (const char *val)
{
        assert(val);

        char upper_val[MAX_NAME_LENGTH] = {};
        for (int i = 0; *(val + i) != '\0'; i++)
                upper_val[i] = (char) toupper(*(val + i));
        static const char *regs_names[N_REGS] = {
#include "../registers.en"
                "R0X"
        };

        for (int i = 0; i < N_REGS; i++)
                if (strstr(upper_val, regs_names[i]))
                        return *(regs_names[i] + 1) - 'A' + 1;

        return NO_REGISTER;
}
#undef DEF_REG

int get_jmp_line (labels_t *labels, char *name)
{
        assert(labels);
        assert(name);

        for (int i = 0; i < MAX_N_LABELS; i++) {
                // printf("+line %s: %d %d\n", labels[i].name, labels[i].line, __LINE__);
                if (!strcmp(labels[i].name, name)) {
                        // printf("+-line %s: %d %d\n", labels[i].name, labels[i].line, __LINE__);
                        return labels[i].line;
                }
        }
        return NO_LABEL;
}

char* get_free_label_name (labels_t *labels)
{
        assert(labels);
        for (int i = 0; i < MAX_N_LABELS; i++)
                if (labels[i].name[0] == '\0')
                        return labels[i].name;

        return nullptr;
}

int create_label (labels_t *labels, const char *cmd, int n_line, int is_call)
{
        assert(labels);
        assert(cmd);
                // printf(" qwerty2 %s %d\n", cmd, n_line);
        int i = 0;
        char name[MAX_NAME_LENGTH] = {};
        strcpy(name, cmd);
        int label_exist = get_jmp_line(labels, name);
                // printf(" qwerty3 %s %d\n", name, n_line);
        while (name[i] != ':' && !is_call)
                i++;
        if (!is_call)
                name[i] = '\0';

        if (label_exist == NO_LABEL) {
                for (i = 0; i < MAX_N_LABELS; i++) {
                        if (labels[i].name[0] == '\0') {
                                strcpy(labels[i].name, name);
                                labels[i].line = n_line;
                                // printf("created label line %s: %d\n", labels[i].name, labels[i].line);
                                // printf("N_LINE %d %s\n", labels[i].line, labels[i].name);
                                break;
                        }
                }
        } else {
                for (i = 0; i < MAX_N_LABELS; i++) {
                        if (strcmp(labels[i].name, name) == 0) {
                                labels[i].line = n_line;
                                // printf("N_LINE %d %s\n", labels[i].line, labels[i].name);
                                break;
                        }
                }
        }

        return 0;
}

void append_txt (char *output_file_name)
{
        assert(output_file_name);

        if (strstr(output_file_name, ".txt") == nullptr)
                for (int i = 0; i < MAX_NAME_LENGTH; i++)
                        if (output_file_name[i] == '\0') {
                                strcpy(output_file_name + i, ".txt");
                                break;
                        }
}

void free_buf (char *buf)
{
        assert(buf);

        for (int i = 0; buf[i] != '\0'; i++) {
                buf[i] = '\0';
        }
}
