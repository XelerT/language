#ifndef ERRORS_HANDLER
#define ERRORS_HANDLER

#include "lexer.h"
#include "config.h"
#include "../tree/token_tree.h"
#include "../tree/tree.h"
#include "tokens.h"

enum lex_error {
        ARGUMENT_ERROR  = 0xBADA6,
        UNEXPECTED_SYMB = 0xBADE1,
        UNEXPECTED_TYPE = 0xBAD11E
};

int check_arg   (const tokens_t *tokens, size_t *tp);
int check_symb  (const tokens_t *tokens, size_t *tp, char symb);
int check_type  (const tokens_t *tokens, const size_t *tp, unsigned int correct_type, const char *explanation, int dump);
void dump_error (const tokens_t *tokens, const size_t *tp, const char *explanation);

#endif /*ERRORS_HANDLER*/
