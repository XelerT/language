#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include\UI.h"

int run_frontend (char *output_file_name, char *input_file_name)
{
        assert_ptr(output_file_name);
        assert_ptr(input_file_name);

        char run_frontend[MAX_NAME_LENGTH * 2] = {};

        if (!sprintf(run_frontend, "front.exe %s %s", output_file_name, input_file_name))
                return NULL_SPRINTF;

        system(run_frontend);
        fprintf(stderr, "\nRun frontend\n\n");

        return 0;
}

int run_backend (char *output_file_name, char *input_file_name)
{
        assert_ptr(output_file_name);
        assert_ptr(input_file_name);

        char run_backend[MAX_NAME_LENGTH * 4] = {};

        if (!sprintf(run_backend, "back.exe %s %s", output_file_name, input_file_name))
                return NULL_SPRINTF;

        system(run_backend);
        fprintf(stderr, "\nRun backend\n\n");

        return 0;
}

int run_asm (char *output_file_name, char *input_file_name)
{
        assert_ptr(output_file_name);
        assert_ptr(input_file_name);

        char run_asm[MAX_NAME_LENGTH * 4] = {};

        if (!sprintf(run_asm, "asm.exe %s %s", output_file_name, input_file_name))
                return NULL_SPRINTF;

        system(run_asm);
        fprintf(stderr, "\nRun assembler\n\n");

        return 0;
}

int run_cpu (char *input_file_name)
{
        assert_ptr(input_file_name);

        char run_cpu[MAX_NAME_LENGTH * 4] = {};

        sprintf(run_cpu, "cpu.exe %s", input_file_name);

        system(run_cpu);
        fprintf(stderr, "\nRun cpu\n\n");

        return 0;
}

int change_file_extension (char *output_file_name, char *asm_code_file_name, const char *extension)
{
        assert_ptr(output_file_name);
        assert_ptr(asm_code_file_name);

        int i = 0;
        while (output_file_name[i] != '.') {
                asm_code_file_name[i] = output_file_name[i];
                i++;
        }
        if (!sprintf(asm_code_file_name + i, "%s", extension))
                return NULL_SPRINTF;

        return 0;
}
