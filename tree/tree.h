#ifndef TREE_H
#define TREE_H

#include "..\include\config.h"
#include "..\include\tokens.h"

enum tree_errors {
        NULL_TREE_PTR = 1,
        NULL_ROOT_PTR = 2,
        NULL_NODE_PTR = 4
};

enum errors {
        NULL_CALLOC = -1,
        NULL_FILE_PTR = -2
};

enum operator_t {
        ADD = '+',
        SUB = '-',
        MUL = '*',
        DIV = '/',
        POW = '^',
        ASS = '='
};

// union elem_t {
//         char   var;
//         int     op;
//         double dbl;
// };

struct node_t {
        node_t *left  = nullptr;
        node_t *right = nullptr;
        size_t indx   =       0;
        int new_node  =       0;
        elem_t data   =       0;
        char name[MAX_NAME_LENGTH] = {}; /*was func*/
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
