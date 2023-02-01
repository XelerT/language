#ifndef BACKEND_H
#define BACHEND_H

#include "../include/utils.h"
#include "tables.h"

const size_t SIZE_T_ERROR = 9999999999;

enum backend_errors {
        INIT_ERROR = 0xBAD141D
};

int create_asm     (tree_t *tree, const char *file_name);
int asm_node       (node_t *node, tab_table_t *table, FILE *output);

int var_init       (table_t *gl_table, table_t *loc_table, int sub_type, char *name);
int asm_while      (FILE *output, tab_table_t *table, node_t *node);
int asm_and        (FILE *output, tab_table_t *table, node_t *node);
int asm_or         (FILE *output, tab_table_t *table, node_t *node);
int asm_operator   (FILE *output, tab_table_t *table, node_t *node);
int func_init      (FILE *output, node_t *node, tab_table_t *table);
int asm_name       (FILE *output, tab_table_t *table, node_t *node);
int asm_func       (FILE *output, node_t *node, tab_table_t *table);
int asm_scan       (FILE *output, node_t *node, tab_table_t *table);
int asm_assignment (FILE *output, node_t *node, tab_table_t *table);
int asm_staff      (FILE *output, node_t *node, tab_table_t *table);
int asm_return     (FILE *output, tab_table_t *table);

size_t count_args  (node_t *node);
int    cast_type   (node_t *node, tab_table_t *table);
size_t find_var    (table_t *table, char *name);

int indent_rbx (FILE *output, size_t n_args);

#endif /*BACKEND_H*/
