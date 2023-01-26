#ifndef UI_H
#define UI_H

#include "config.h"
#include "utils.h"
#include "..\log\log.h"

int run_frontend          (char *output_file_name, char *input_file_name);
int run_backend           (char *output_file_name, char *input_file_name);
int run_asm               (char *output_file_name, char *input_file_name);
int run_cpu               (char *input_file_name);
int change_file_extension (char *output_file_name, char *asm_code_file_name, const char *extension);

#endif /*UI_H*/
