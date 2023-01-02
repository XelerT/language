#ifndef TOKENS_H
#define TOKENS_H

enum types_t {
        VARIABLE    =  1,
        NUMBER      =  90,
        FUNC        =  4,
        CONST       =  5,
        RELATIVE_OP =  7,
        WORD        =  9,
        END_FILE    = 14
};

#define WT(type,val,...)    type = val,

enum word_types_t {
        #include "words_types.wt"
        UNKNOWN_TYPE = 0xFF
};

#undef WT

#define SYMB(key,arg,num,type,...) type = num,

enum symb_types_t {
        #include "symbles.sym"
        ASSIGNMENT  =  255
};

#undef SYMB

struct token_arg_t {
        unsigned char   type     = 0;
        char   sub_type = 0;
        elem_t val      = 0;
        char name[MAX_NAME_LENGTH] = {'\0'};

        graph_node_atr_t atr = {};
};

struct tokens_t {
        token_arg_t *tok_args = nullptr;
        size_t capacity = 0;
        size_t size     = 0;
};

#endif /*TOKENS_H*/
