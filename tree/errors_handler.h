#ifndef ERRORS_HANDLER
#define ERRORS_HANDLER

#include "..\include\lexer.h"
#include "..\include\config.h"
#include "token_tree.h"
#include "tree.h"
#include "..\include\tokens.h"

enum lex_error {
        ARGUMENT_ERROR  = 0xBADA6,
        UNEXPECTED_SYMB = 0xBADE1
};

int check_arg  (const tokens_t *tokens, size_t *tp);
int check_symb (const tokens_t *tokens, size_t *tp, char symb);

#endif /*ERRORS_HANDLER*/
