#include <stdio.h>
#include <cstdlib>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include "assembler.h"
#include "../../../include/utils.h"
#include "../../../debug/debug.h"

int main (int argc, char *argv[])
{
        if (int i = check_argv(argc, argv))
                return i;

        FILE *src_code    = nullptr;
        FILE *output_code = nullptr;
        char output_file_name[MAX_NAME_LENGTH] = {};
        src_code = fopen(argv[2], "r");
        if (!src_code) {
                printf("Source file pointer is null.\n");
                return NO_SOURCE;
        }
        strcpy(output_file_name, argv[1]);
        append_txt(output_file_name);
        output_code = fopen(output_file_name, "w");
        if (!output_code) {
                fprintf(stderr, "Output_code_file(%s) didn't open.", output_file_name);
                return NO_SOURCE;
        }
        // fprintf(output_code, "");
        code_t code = {};
        labels_t labels[MAX_N_LABELS] = {};

        get_code(src_code, &code, argv[2]);
        fclose(src_code);

        int *asm_code = (int*) calloc(code.n_lines * 2 * 10, sizeof(int));
        if (!asm_code) {
                printf("Calloc return null pointer.\n");
                return NULL_CALLOC;
        }
        pre_asm(&code);

        if (convert_code(&code, output_code, 0, labels, asm_code) == NO_LABEL) {
                fprintf(stderr, "No label\n");
                return NO_LABEL;
        }
        free(code.buf);
        free(code.lines);
        free(code.cmds);
        free(asm_code);
        fclose(output_code);
        return 0;
}
