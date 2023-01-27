#include <stdio.h>
#include <stdlib.h>
#include "tables.h"

int tab_table_ctor (tab_table_t *table, size_t capacity)
{
        assert_ptr(table);

        table_t loc_tables[DEFAULT_N_TABLS * 4] = {};
        table_ctor(table->gl_table, DEFAULT_N_VARS, DEFAULT_N_FUNCS);


        table->loc_tables = (table_t**) calloc(DEFAULT_N_TABLS, sizeof(table_t*));
        if (!table->loc_tables) {
                log(1, "Calloc for table of tables is NULL");
                return NULL_CALLOC;
        }
        table->loc_cap = DEFAULT_N_TABLS;

        for (size_t i = 0; i < capacity; i++) {
                table_ctor(loc_tables + i, DEFAULT_N_VARS, DEFAULT_N_FUNCS);
                table->loc_tables[i] = loc_tables + i;
        }

        return 0;
}

int resize_tab_table (tab_table_t *table)
{
        assert_ptr(table);

        if (table->loc_size * 2 > table->loc_cap) {
                table_t **temp_loc_tables = (table_t**) realloc(table->loc_tables, table->loc_cap * 2);
                if (temp_loc_tables) {
                        table->loc_tables = temp_loc_tables;
                        table->loc_cap  *= 2;
                } else {
                        log_error(1, "!Realloc returned null!");
                        return REALLOC_ERR;
                }
                log(1, "<span style = \"color: green; font-size:10px;\">Resized loc_tables</span>");
        }

        resize_table(table->gl_table);
        for (size_t i = 0; i < table->loc_size; i++) {
                resize_table(table->loc_tables[i]);
        }

        return 0;
}

int tab_table_dtor (tab_table_t *table)
{
        assert_ptr(table);

        table_dtor(table->gl_table);

        if (table->loc_tables) {
                free(table->loc_tables);
                table->loc_tables = nullptr;
                log(1, "<span style = \"color: green; font-size:14px;\"> Freed local tables </span>");
        } else {
                log(1, "<span style = \"color: #436AE8; font-size:14px;\"> Destructor of table tried to free variables pointer </span>");
        }

        return 0;
}

int table_ctor (table_t *table, size_t var_cap, size_t func_cap)
{
        assert_ptr(table);

        table->vars = (variable_t*) calloc(var_cap, sizeof(variable_t));
        if (!table->vars) {
                log(1, "Calloc for variable table if NULL");
                return NULL_CALLOC;
        }
        table->var_cap = var_cap;

        table->funcs = (function_t*) calloc(func_cap, sizeof(function_t));
        if (!table->funcs) {
                log(1, "Calloc for function table if NULL");
                return NULL_CALLOC;
        }
        table->func_cap = func_cap;

        return 0;
}

int resize_table (table_t *table)
{
        assert_ptr(table);

        variable_t *temp = nullptr;

        if (table->var_size + 10 >= table->var_cap) {
                temp = (variable_t*) realloc(table->vars, sizeof(variable_t) * table->var_cap * 2);
                if (temp) {
                        table->vars = temp;
                } else {
                        log_error(1, "!Realloc returned null!");
                        return REALLOC_ERR;
                }
                log(1, "<span style = \"color: green; font-size:10px;\">Resized var_table</span>");
        }

        function_t *temp2 = nullptr;
        if (table->func_size + 10 >= table->func_cap) {
                temp2 = (function_t*) realloc(table->funcs, sizeof(function_t) * table->func_cap * 2);
                if (temp2 || table->func_cap == 0) {
                        table->funcs = temp2;
                } else {
                        log_error(1, "!Realloc returned null!");
                        return REALLOC_ERR;
                }
                log(1, "<span style = \"color: green; font-size:10px;\">Resized func_table</span>");
        }

        return 0;
}

int table_dtor (table_t *table)
{
        assert_ptr(table);

        if (table->vars) {
                free(table->vars);
                table->vars = nullptr;
                log(1, "<span style = \"color: green; font-size:14px;\"> Freed variable table </span>");
        } else {
                log(1, "<span style = \"color: #436AE8; font-size:14px;\"> Destructor of table tried to free variables pointer </span>");
        }
        if (table->funcs) {
                free(table->funcs);
                table->funcs = nullptr;
                log(1, "<span style = \"color: green; font-size:14px;\"> Freed function table </span>");
        } else {
                log(1, "<span style = \"color: #436AE8; font-size:14px;\"> Destructor of table tried to free functions pointer </span>");
        }

        return 0;
}
