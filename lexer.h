#ifndef LEXER_H
#define LEXER_H

#include "tree\text_tree.h"

enum lexer_errors {
        NULL_CAP  = 0xFFF,
        NULL_FILE =  0xFF,
        ARG_ERROR =   0xF
};

enum consts {
        RESIZED = 0xCA46E,
};

struct token_arg_t {
        char type = 0;
        char name[MAX_NAME_LENGTH] = {'\0'};

        graph_node_atr_t atr = {};
};

struct tokens_t {
        token_arg_t *tok_args = nullptr;
        size_t capacity = 0;
        size_t size     = 0;
};

tokens_t* tokens_ctor (tokens_t *tokens, size_t capacity);
int resize_tokens (tokens_t *tokens);
// int insert_token (tokens_t *tokens, token_arg_t *token_arg);
int get_tokens (tokens_t *tokens, const char *file_name);
int get_arg (token_arg_t *token, char *buf, size_t *ip);          //add assert errors
int get_word(token_arg_t *token, char *buf, size_t *ip);
int get_number(token_arg_t *token, char *buf, size_t *ip);
int get_relat_op(token_arg_t *token, char *buf, size_t *ip);
int get_punct (token_arg_t *token, char *buf, size_t *ip);

#endif /*LEXER_H*/
