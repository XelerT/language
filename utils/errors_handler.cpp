#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include "..\include\errors_handler.h"

int check_arg (const tokens_t *tokens, size_t *tp)
{
        assert_ptr(tokens);
        assert_ptr(tp);

        unsigned int type = tokens->tok_args[*tp].type;
        char        *name = tokens->tok_args[*tp].name;

        if (type != NUMBER && type != NAME) {
                log_error(3, "Expected argument but next element has %d(%s) type", type, name);
                return ARGUMENT_ERROR;
        }

        return 0;
}

int check_symb (const tokens_t *tokens, size_t *tp, char symb)
{
        assert_ptr(tokens);
        assert_ptr(tp);

        unsigned int type = tokens->tok_args[*tp].type;
        char        *name = tokens->tok_args[*tp].name;

        if (symb != name[0]) {
                log_error(4, "Expected %c but next element has %d(%s) type", symb, type, name);
                return UNEXPECTED_SYMB;
        }

        return 0;
}

int check_type (const tokens_t *tokens, const size_t *tp, unsigned int correct_type, const char *explanation, int dump)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(explanation);

        if (tokens->tok_args[*tp].type == correct_type) {
                return 0;
        } else {
                if (dump)
                        dump_error(tokens, tp, explanation);
                return UNEXPECTED_TYPE;
        }
}

void dump_error (const tokens_t *tokens, const size_t *tp, const char *explanation)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(explanation);

        size_t i = *tp;

        while (tokens->tok_args[i].n_line == tokens->tok_args[i - 1].n_line && i > 0)
                i--;

        if (i == 1)
                i = 0;

        size_t j = tokens->tok_args[i].n_char;
        size_t start_j = j;
        do {
                while (j <= tokens->tok_args[i].n_char) {
                        putchar(tokens->inputed_text[j]);
                        j++;
                }
                i++;
        } while (tokens->tok_args[i].n_line == tokens->tok_args[i - 1].n_line);
        putchar('\n');

        j = j - start_j;
        for (size_t k = 0; k < j; k++) {
                putchar(' ');
        }
        putchar('^');
        putchar('\n');
        for (size_t k = 0; k < j; k++) {
                putchar(' ');
        }
        printf("|\n");

        printf("%s (line: %lld, symbal: %lld)\n", explanation, tokens->tok_args[*tp].n_line, j);
}
