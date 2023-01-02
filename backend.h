#ifndef BACKEND_H
#define BACHEND_H

#include "include\utils.h"

const size_t SIZE_T_ERROR = 9999999999;

enum backend_errors {
        INIT_ERROR = 0xBAD141D
};

struct variable_t {
        char name[MAX_NAME_LENGTH] = {'\0'};
        elem_t val = 0;
        int number = 0;
};

struct table_t {
        variable_t *vars    = nullptr;
        size_t var_capacity = 0;
        size_t var_size     = 0;
        size_t if_size      = 0;
};

int create_asm (tree_t *tree, char *file_name);
int asm_node (node_t *node, table_t *gl_table, FILE *output);
int var_init (table_t *table, int sub_type, char *name);
int resize_table (table_t *table);
int table_ctor (table_t *table, size_t capacity);
size_t find_var (table_t *table, char *name);

#endif /*BACKEND_H*/
