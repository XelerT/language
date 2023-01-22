#include <stdio.h>
#include "dis_asm.h"

#define DEF_STND_CMD(name,num,arg,...) DEF_CMD(name,num,arg,__VA_ARGS__)
#define DEF_J_CMD(name,num,arg,...)    DEF_CMD(name,num,666,__VA_ARGS__)
#define DEF_CMD(name,num,arg,...)                                                                                       \
                                case num:                                                                               \
                                        PP_CASE(coeff)                                                                  \
                                        else if (arg == 666) {                                                          \
                                               fprintf(output, "%s %d\n", #name, code->cmds[++ip]);                       \
                                        } else {                                                                        \
                                                fprintf(output, "%s\n", #name);                                         \
                                        }                                                                               \
                                        ip++;                                                                           \
                                        break;

#define PP_CASE(coeff)  if ((code->cmds[ip] & MASK_CMD) == CMD_PUSH) {                                                  \
                                if (code->cmds[ip] & ARG_REG) {                                                         \
                                        fprintf(output, "push r%cx\n", 'a' +  code->cmds[++ip] - 1);                    \
                                } else if (code->cmds[ip] & ARG_RAM) {                                                  \
                                        if (code->cmds[ip] & ARG_REG)                                                   \
                                                fprintf(output, "push [r%cx]\n", 'a' +  code->cmds[++ip]  - 1);         \
                                        if (code->cmds[ip] & ARG_IMMED) {                                               \
                                                fprintf(output, "push [%d]\n", code->cmds[++ip]);                       \
                                        }                                                                               \
                                } else  if (code->cmds[ip] & ARG_IMMED) {                                               \
                                        fprintf(output, "push %d\n", code->cmds[++ip]);                                 \
                                }                                                                                       \
                        } else if ((code->cmds[ip] & MASK_CMD) == CMD_POP) {                                            \
                                if (code->cmds[ip] & ARG_RAM) {                                                         \
                                        if (code->cmds[ip] & ARG_REG)                                                   \
                                                fprintf(output, "pop [r%cx]\n", 'a' + code->cmds[++ip] - 1);            \
                                        if (code->cmds[ip] & ARG_IMMED)                                                 \
                                                fprintf(output, "pop [%d]\n", code->cmds[++ip]);                        \
                                }                                                                                       \
                                if (code->cmds[ip] & ARG_REG) {                                                         \
                                        fprintf(output, "push r%cx\n", 'a' + code->cmds[++ip] - 1);                     \
                                }                                                                                       \
                        }

#define CALL_ stack_push(&call_stack, ip + 2);

int dis_asm_code (code_t *code, const char *input_file_name)
{
        assert(code);

        int ip = 0;
        char output_file_name[MAX_NAME_LENGTH] = {'\0'};
        make_output_file_name(output_file_name, input_file_name);
        FILE* output = fopen(output_file_name, "w");
        if (!output)
                return NO_SOURCE;

        while (code->cmds[ip] != CMD_HLT) {
                switch (code->cmds[ip] & MASK_CMD) {
#include "..\instructions.en"
                default:
                        if (code->cmds[ip] == CMD_LABEL)
                                fprintf(output, "LABEL\n");
                        ip++;

                }
        }
        fprintf(output, "hlt");

        fclose(output);
        return 0;
}

#undef DEF_STND_CMD
#undef DEF_J_CMD
#undef DEF_CMD
#undef CALL_
#undef PP_CASE
#undef PUSH
#undef POP
#undef JMP

int divide_cmds (code_t *code)
{
        assert(code);

        int *cmd_list = (int*) calloc(code->n_chars / 2 + 1, sizeof(int));

        if (!code) {
                printf("Calloc returned NULL.\n");
                return NULL_CALLOC;
        }

        int cmd = 0;
        int ip = 0;
        int n_chars = 0;
        int i = 0;

        while (cmd != CMD_HLT) {
                sscanf(code->buf + i, "%d %n", &cmd, &n_chars);
                cmd_list[ip++] = cmd;
                i += n_chars;
        }
        code->cmds = cmd_list;

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

int source_file_ctor (FILE *source_code, char *input_file_name, char *argv)
{
        assert(input_file_name);
        assert(argv);

        strcpy(input_file_name, argv);
        append_txt(input_file_name);
        source_code = fopen(input_file_name, "r");
        if (!source_code)
                return NO_SOURCE;

        return 0;
}

void make_output_file_name(char *output_file_name, const char *input_file_name)
{
        assert(output_file_name);
        assert(input_file_name);

        sprintf(output_file_name, "dis_asmed_");
        for (int i = 0; input_file_name[i] != '\0'; i++) {
                output_file_name[i + 10] = input_file_name[i];
        }
}
