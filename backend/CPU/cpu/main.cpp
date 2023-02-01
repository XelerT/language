#include <stdio.h>
#include "cpu.h"
#include "../../../include/utils.h"
#include "../../../debug/debug.h"

int main (int argc, char *argv[])
{
        if (argc != 2) {
                fprintf(stderr, "Nuzna 2 argumenta\n");
                return EXIT_FAILURE;
        }

        code_t code = {};
        stack stk   = {};
        if (stack_ctor(&stk))
                return CTOR_ERROR;

        cpu_t cpu = {};

        FILE *source_code = nullptr;
        char input_file_name[MAX_NAME_LENGTH] = {};
        if (source_file_ctor(input_file_name, argv[1])) {
                return NO_SOURCE;
        }

        source_code = fopen(input_file_name, "r");
        get_code(source_code, &code, input_file_name);

        int error = 0;

        if ((error = divide_cmds(&code))) {
                free(code.buf);
                free(code.lines);
                free(code.cmds);
                free_stack(&stk);
                fclose(source_code);
                return error;
        }

        error = execute_code(&code, &stk, &cpu);

        free(code.buf);
        free(code.lines);
        free(code.cmds);
        free_stack(&stk);
        fclose(source_code);

        return error;
}
