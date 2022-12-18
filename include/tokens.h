#ifndef TOKENS_H
#define TOKENS_H

enum types_t {
        VARIABLE    =  1,
        OPERATOR    =  2,
        NUMBER      =  3,
        FUNC        =  4,
        CONST       =  5,
        ASSIGNMENT  =  6,
        RELATIVE_OP =  7,
        END_LINE    =  8,
        WORD        =  9,
        PM_OPERATOR = 10,
        MD_OPERATOR = 11,
        CL_BRACKET  = 12,
        OP_BRACKET  = 13,
        END_FILE    = 14
};

struct token_arg_t {
        char type = 0;
        char name[MAX_NAME_LENGTH] = {'\0'};
        elem_t val = 0;

        graph_node_atr_t atr = {};
};

struct tokens_t {
        token_arg_t *tok_args = nullptr;
        size_t capacity = 0;
        size_t size     = 0;
};

#endif /*TOKENS_H*/
