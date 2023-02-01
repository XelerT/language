#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <locale.h>

#include "text_tree.h"

int get_text (FILE *input, text_t *text, const char *file_name)
{
        if (!input) {
                log(1, "File pointer is null.");
                return NULL_FILE_PTR;
        }
        if (!text) {
                log(1, "Text pointer is null.");
                return NULL_TEXT_PTR;
        }

        struct stat file = {};
        if (stat(file_name, &file) < 0)
                return FILE_ERR;

        size_t n_chars = 0;
        char *buf = (char*) calloc(file.st_size + 1, sizeof(char));
        if (!buf) {
                log(1, "Calloc returned NULL.");
                return NULL_CALLOC;
        }

        n_chars = fread(buf, sizeof(char), file.st_size, input);
        text->n_chars = n_chars;

        text->buf = buf;
        text->n_lines = file.st_size - n_chars;
        if (!n_chars) {
                log(1, "Input file is empty.");
                return EMPTY_FILE;
        }
        return 0;
}

void replace_n (text_t *text)
{
        if (!text) {
                log(1, "Text pointer is null.");
                return;
        }

        char *buf = text->buf;

        for (size_t i = 0; i < text->n_chars; i++)
                if (text->buf[i] == '\n')
                        buf[i] = '\0';
}

void divide_text (text_t *text)
{
        if (!text) {
                log(1, "Text pointer is null.");
                return;
        }

        char *buf = text->buf;
        buf[text->n_chars + 1] = '\0';

        line_t *lines = (line_t*) calloc(text->n_lines + 1, sizeof(line_t));
        if (!lines) {
                log(1, "Calloc returned NULL.");
                return;
        }
        text->lines = lines;
        for (size_t i = 0; i < text->n_lines; i++) {
                lines[i].ptr = buf;
                while (*buf != '\n' && *buf != '\0')
                        buf++;
                buf++;
                lines[i].length = buf - lines[i].ptr;
        }
}
