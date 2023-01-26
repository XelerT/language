#ifndef LEXER_H
#define LEXER_H

#include "..\tree\text_tree.h"
#include "tokens.h"

enum lexer_errors {
        NULL_CAP  = 0xFFF,
        NULL_FILE =  0xFF,
        ARG_ERROR =   0xF
};

enum consts {
        RESIZED = 0xCA46E,
};

tokens_t* tokens_ctor (tokens_t *tokens, size_t capacity);
int resize_tokens (tokens_t *tokens);
// int insert_token (tokens_t *tokens, token_arg_t *token_arg);
int get_tokens   (tokens_t   *tokens, const char *file_name);
int get_arg      (tokens_t   *tokens, char *buf, size_t *ip, size_t tp);
int get_word     (token_arg_t *token, char *buf, size_t *ip);
int get_number   (token_arg_t *token, char *buf, size_t *ip);
int get_relat_op (token_arg_t *token, char *buf, size_t *ip);
int get_punct    (token_arg_t *token, char *buf, size_t *ip);

#endif /*LEXER_H*/
