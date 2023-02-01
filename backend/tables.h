#ifndef TABLES_H
#define TABLES_H

#include "../include/utils.h"
#include "../log/log.h"

struct variable_t {
        char name[MAX_NAME_LENGTH] = {'\0'};
        elem_t val                 =      0;
        int number                 =      0;
        unsigned int type          =      0;
};

struct function_t {
        char name[MAX_NAME_LENGTH] = {'\0'};
        size_t n_args              =      0;
        unsigned int type          =      0;
        int has_return             =      0;
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

        size_t rel_op_size   = 0;
};

struct tab_table_t {
        table_t *gl_table     = {};
        table_t **loc_tables  = nullptr;
        size_t loc_cap        =  0;
        size_t loc_size       =  0;
};

int table_ctor       (table_t *table, size_t var_cap, size_t func_cap);
int resize_table     (table_t *table);
int table_dtor       (table_t *table);

int tab_table_ctor   (tab_table_t *table, size_t capacity);
int resize_tab_table (tab_table_t *table);
int tab_table_dtor   (tab_table_t *table);

#endif /*TABLES_H*/
