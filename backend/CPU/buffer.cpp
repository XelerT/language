#include <stdio.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdlib.h>
#include "buffer.h"

void get_code (FILE *input, code_t *code, char *code_file)
{
        assert(input);
        assert(code);
        assert(code_file);

        struct stat file = {};
        if (stat(code_file, &file) < 0)
                return;
        size_t n_chars = 0;
        char *buf = (char*) calloc(file.st_size + 1, sizeof(char));
        if (!buf) {
                printf("Calloc returned NULL.\n");
                return;
        }

        n_chars = fread(buf, sizeof(char), file.st_size, input);
        code->n_chars = n_chars;

        for (size_t i = 0; i < n_chars; i++)
                if (buf[i] == '\n')
                        buf[i] = '\0';

        code->buf = buf;
        size_t n_lines = file.st_size - n_chars;
        code->n_lines = n_lines;
        divide_code (code);
}

void divide_code (code_t *code)
{
        assert(code);
        char *buf = code->buf;
        assert(buf);

        buf[code->n_chars + 1] = '\0';

        line_t *lines = (line_t*) calloc(code->n_lines + 1, sizeof(line_t));
        if (!lines) {
                printf("Calloc returned NULL.\n");
                return;
        }
        code->lines = lines;
        assert(lines);
        assert(code->lines);

        for (size_t i = 0; i < code->n_lines; i++) {
                if (*(buf + 1) != '\n' && *(buf + 1) != '\0') {
                        lines[i].ptr = buf;
                        while (*buf != '\n' && *buf != '\0')
                                buf++;
                        buf++;
                        lines[i].length = buf - lines[i].ptr;
                } else
                        buf++;
        }
        assert(lines);
}
