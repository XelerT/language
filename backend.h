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

struct function_t {
        char name[MAX_NAME_LENGTH] = {'\0'};
        size_t n_args = 0;
        char type     = 0;
};

struct table_t {
        variable_t *vars     = nullptr;
        size_t var_cap       = 0;
        size_t var_size      = 0;

        size_t if_size       = 0;
        size_t and_size      = 0;
        size_t or_size       = 0;
        size_t while_size    = 0;

        function_t *funcs    = nullptr;
        size_t func_cap      = 0;
        size_t func_size     = 0;
};

int create_asm    (tree_t *tree, const char *file_name);
int base_asm_node      (node_t *node, table_t *gl_table, table_t *loc_table, FILE *output);
int var_init      (table_t *gl_table, table_t *loc_table, int sub_type, char *name);
int resize_table  (table_t *table);
int table_ctor    (table_t *table, size_t var_cap, size_t func_cap);
int table_dtor    (table_t *table);
size_t find_var   (table_t *table, char *name);
int asm_while     (FILE *output, table_t *gl_table, node_t *node);
int asm_and       (FILE *output, table_t *gl_table, node_t *node);
int asm_or        (FILE *output, table_t *gl_table, node_t *node);
int asm_operator  (FILE *output, table_t *gl_table, node_t *node);
int func_init     (FILE *output, node_t *node, table_t *table);
int asm_func      (FILE *output, node_t *node, table_t *table);
size_t count_args (node_t *node, table_t *table);


#endif /*BACKEND_H*/
