#ifndef TOKENS_H
#define TOKENS_H

#define TYPE(name,num) name = num,

enum types_t {
        #include "types.tp"
};

#undef TYPE

#define WT(type,val,...)    type = val,

enum word_types_t {
        #include "words_types.wt"
        UNKNOWN_TYPE = 0xFF
};

#undef WT

#define SYMB(key,arg,num,type,...) type = num,

enum symb_types_t {
        #include "symbles.sym"
        ASSIGNMENT  =  254
};

#undef SYMB

struct token_arg_t {
        unsigned int type = 0;
        char   sub_type   = 0;
        elem_t val        = 0;
        char name[MAX_NAME_LENGTH] = {'\0'};

        graph_node_atr_t atr = {};
};

struct tokens_t {
        token_arg_t *tok_args = nullptr;
        size_t capacity       = 0;
        size_t size           = 0;
};

#endif /*TOKENS_H*/
