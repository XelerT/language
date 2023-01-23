#include <stdio.h>
#include "cpu.h"

#define DEF_STND_CMD(name,num,arg,...) DEF_CMD(name,num,arg,__VA_ARGS__)
#define DEF_J_CMD(name,num,arg,...)    DEF_CMD(name,num,arg,__VA_ARGS__)
#define DEF_CMD(name,num,arg,...)                                      \
                                case num:                              \
                                        if (arg) __VA_ARGS__           \
                                        ip++;                          \
                                        break;
#define PP_CASE(coeff)  if ((code->cmds[ip] & MASK_CMD) == CMD_PUSH) {                                                  \
                                if ((code->cmds[ip] & ARG_REG) && !(code->cmds[ip] & ARG_RAM)) {                        \
                                        stack_push(stk, cpu->registers[code->cmds[++ip]]);                              \
                                } else if (code->cmds[ip] & ARG_RAM) {                                                  \
                                        if (code->cmds[ip] & ARG_REG) {                                                 \
                                                stack_push(stk, cpu->RAM[cpu->registers[code->cmds[++ip]]]);            \
                                        }                                                                               \
                                        if (code->cmds[ip] & ARG_IMMED) {                                               \
                                                stack_push(stk, cpu->RAM[code->cmds[++ip]]);                            \
                                        }                                                                               \
                                } else  if (code->cmds[ip] & ARG_IMMED) {                                               \
                                        stack_push(stk, code->cmds[++ip]);                                              \
                                }                                                                                       \
                        } else if ((code->cmds[ip] & MASK_CMD) == CMD_POP) {                                            \
                                if (code->cmds[ip] & ARG_RAM) {                                                         \
                                        if (code->cmds[ip] & ARG_REG)                                                   \
                                                cpu->RAM[cpu->registers[code->cmds[++ip]]] = stack_pop(stk);            \
                                        if (code->cmds[ip] & ARG_IMMED) {                                               \
                                                cpu->RAM[code->cmds[++ip]] = stack_pop(stk);                            \
                                        }                                                                               \
                                }                                                                                       \
                                if (code->cmds[ip] & ARG_REG) {                                                         \
                                        cpu->registers[code->cmds[++ip]] = stack_pop(stk);                              \
                                }                                                                                       \
                        }

#define CALL_ stack_push(&call_stack, ip + 2);

#define PUSH(arg) stack_push(stk, arg)
#define POP stack_pop(stk)

#define JMP {ip = code->cmds[ip + 1];}

int execute_code (code_t *code, stack *stk, cpu_t *cpu)
{
        assert(code);
        assert(stk);
        assert(cpu);

        int ip = 0;
        int temp = 0;
        stack call_stack = {};

        if (stack_ctor(&call_stack))
                return CTOR_ERROR;

        while (code->cmds[ip] != CMD_HLT) {
                // printf("%d cmd: %d\n", ip, code->cmds[ip]);
                switch (code->cmds[ip] & MASK_CMD) {
#include "..\instructions.en"
                case CMD_CALL_LABEL:
                        while (code->cmds[ip] != CMD_RET)
                                ip++;
                        ip++;
                        break;
                default:
                        ip++;
                }
        }

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

        int *cmd_list = (int*) calloc(code->n_chars * 10, sizeof(int));
        if (!code) {
                printf("Calloc returned NULL.\n");
                return NULL_CALLOC;
        }
        int cmd = 0;
        int ip = 0;
        int n_chars = 0;

        for (size_t i = 0; i < code->n_chars; ) {
                sscanf(code->buf + i, "%d %n", &cmd, &n_chars);
                cmd_list[ip++] = cmd;
                i += n_chars;
        }
        code->cmds = cmd_list;

        return 0;
}

int source_file_ctor (char *input_file_name, char *argv)
{
        assert(input_file_name);
        assert(argv);

        strcpy(input_file_name, argv);
        append_txt(input_file_name);
        // source_code = fopen(input_file_name, "r");
        // if (!source_code)
        //         return NO_SOURCE;

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
