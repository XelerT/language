#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const int MAX_NAME_LENGTH = 64;

int main (int argc, char *argv[])
{

        if (argc != 3) {
                fprintf(stderr, "You need to enter 2 arguments.\n");
                return -1;
        }
        char  input_file_name[MAX_NAME_LENGTH] = {'\0'};
        char output_file_name[MAX_NAME_LENGTH] = {'\0'};
        strcpy(input_file_name,  argv[2]);
        strcpy(output_file_name, argv[1]);

        char run_lang[MAX_NAME_LENGTH * 2] = {};
        sprintf(run_lang, "make run_lang LANG_OUT=%s LANG_CODE=%s", output_file_name, input_file_name);

        system(run_lang);
        fprintf(stderr, "\nRunned language\n");


        char run_asm[MAX_NAME_LENGTH * 2] = {};
        sprintf(run_asm, "make run_asm ASM_OUT=%s ASM_CODE=%s", "output.txt", output_file_name);

        // system(run_asm);
        // fprintf(stderr, "\nRunned assembler\n");

        char run_cpu[MAX_NAME_LENGTH * 2] = {};
        sprintf(run_cpu, "make run_cpu CPU_CODE=%s", "output.txt");

        // system(run_cpu);
        // fprintf(stderr, "\nRunned cpu\n");

        return 0;
}
