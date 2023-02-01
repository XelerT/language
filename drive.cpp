#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/UI.h"

int main (int argc, char *argv[])
{
        if (argc != 3) {
                fprintf(stderr, "You need to enter 2 arguments.\n");
                return -1;
        }

        if (strlen(argv[1]) > MAX_NAME_LENGTH) {
                fprintf(stderr, "Your output file name is too long.");
                return TOO_LONG;
        }

        if (strlen(argv[2]) > MAX_NAME_LENGTH) {
                fprintf(stderr, "Your input file name is too long.");
                return TOO_LONG;
        }

        char asm_code_file_name[MAX_NAME_LENGTH * 2] = {};

        change_file_extension(argv[1], asm_code_file_name, ".zbor");

        run_frontend(asm_code_file_name, argv[2]);

        run_backend(asm_code_file_name, argv[2]);

        char cpu_code_file_name[MAX_NAME_LENGTH * 2] = {};

        change_file_extension(argv[1], cpu_code_file_name, ".prac");
        run_asm(cpu_code_file_name, asm_code_file_name);

        run_cpu(cpu_code_file_name);

        return 0;
}
