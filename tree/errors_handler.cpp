#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include "errors_handler.h"

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
