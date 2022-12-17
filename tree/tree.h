#ifndef TREE_H
#define TREE_H

#include "..\debug\debug.h"
#include "..\log\log.h"

enum tree_errors {
        NULL_TREE_PTR = 1,
        NULL_ROOT_PTR = 2,
        NULL_NODE_PTR = 4
};

enum errors {
        NULL_CALLOC = -1,
        NULL_FILE_PTR = -2
};

struct graph_node_atr_t {
        const char *shape = "rectangle";
        const char *style = "rounded";
        const char *fixedsize = "false";
        const char *fillcolor = "#00b899";
        int height = 3;
        int width = 2;
        int fontsize = 30;
        int penwidth = 5;
};

enum operator_t {
        ADD = '+',
        SUB = '-',
        MUL = '*',
        DIV = '/',
        POW = '^',
        ASS = '='
};

union elem_t {
        char   var;
        int     op;
        double dbl;
};

enum types_t {
        VARIABLE    =  1,
        OPERATOR    =  2,
        NUMBER      =  3,
        FUNC        =  4,
        CONST       =  5,
        ASSIGNMENT  =  6,
        RELATIVE_OP =  7,
        END_LINE    =  8,
        WORD        =  9
};

const int MAX_NAME_LENGTH = 50;

struct node_t {
        node_t *left  = nullptr;
        node_t *right = nullptr;
        size_t indx   =       0;
        int new_node  =       0;
        elem_t data;
        char func[MAX_NAME_LENGTH] = {};
        char type = 0;

        graph_node_atr_t atr = {};
};

struct tree_t {
        node_t *root = nullptr;
        size_t size  =       0;
};

int tree_ctor (tree_t *tree);
node_t* tree_insert (const node_t *node);
int tree_dtor (tree_t *tree);
void free_nodes (node_t *node);

#endif /*TREE_H*/
