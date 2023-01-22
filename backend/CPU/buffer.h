#ifndef BUFFER_H
#define BUFFER_H

struct line_t
{
        char *ptr = nullptr;
        size_t length = 0;
};

struct code_t
{
        size_t n_lines = 0;
        size_t n_chars = 0;
        char *buf = nullptr;
        line_t *lines = nullptr;
        int *cmds = nullptr;
};

void get_code (FILE *input, code_t *code, char *code_file);
void divide_code (code_t *code);

#endif /*BUFFER_H*/
