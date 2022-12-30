#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree\tree.h"
#include "backend.h"

int create_asm (tree_t *tree, char *file_name)
{
        assert_ptr(tree);

        table_t global_table = {};
        if (table_ctor(&global_table, 20))
                return NULL_CALLOC;

        FILE *output = fopen(file_name, "w");

        asm_node(tree->root, &global_table, output);

        fprintf(output, "hlt\n");
        fclose(output);
        return 0;
}

int asm_node (node_t *node, table_t *gl_table, FILE *output)
{
        assert_ptr(node);
        assert_ptr(gl_table);

        if (node->left) {
                asm_node(node->left, gl_table, output);
        }
        if (node->right) {
                asm_node(node->right, gl_table, output);
        }

        size_t indent = 0;
        switch (node->type) {
        case NAME:
                indent = find_var(gl_table, node->name);
                if (indent == SIZE_T_ERROR) {
                        log(2, "<span style = \"color: red; font-size:16px;\">!Variable %s was not initialized!</span>", node->name);
                        return INIT_ERROR;
                }
                fprintf(output, "push [rax + %lld]\n", indent);
                break;
        case VARIABLE:
                if (var_init(gl_table, node->sub_type, node->name))
                        return INIT_ERROR;
                log(2, "Initialized \"%s\" variable", node->name);
                break;
        case ASSIGNMENT:
                indent = find_var(gl_table, node->left->name);
                fprintf(output, "pop [rax + %lld]\n", indent);
                log(2, "Assigned to rax + %d", indent);
                break;
        case NUMBER:
                fprintf(output, "push %d\n", node->data);
                log(2, "Pushed %d", node->data);
                break;
        case ADD_OPERATOR:
                fprintf(output, "add\n");
                log(1, "add");
                break;
        case SUB_OPERATOR:
                fprintf(output, "sub\n");
                log(1, "sub");
                break;
        case MUL_OPERATOR:
                fprintf(output, "mul\n");
                log(1, "mul");
                break;
        case DIV_OPERATOR:
                fprintf(output, "div\n");
                log(1, "div");
                break;
        default:
                log(1, "Default switch case");
        }

        return 0;
}

size_t find_var (table_t *table, char *name)
{
        assert_ptr(table);

        for (size_t i = 0; i < table->size; i++) {
                if (!strcmp(name, table->vars[i].name)) {
                        return i;
                }
        }

        return SIZE_T_ERROR;
}

int var_init (table_t *table, int sub_type, char *name)
{
        assert_ptr(table);
        assert_ptr(name);

        for (size_t i = 0; i < table->size; i++) {
                if (!strcmp(name, table->vars[i].name)) {
                        log(2, "<span style = \"color: red; font-size:16px;\">!Variable %s already initialized!</span>", name);
                        return INIT_ERROR;
                }
        }

        if (resize_table(table))
                return REALLOC_ERR;

        table->vars[table->size].val = 0;
        strcpy(table->vars[table->size].name, name);
        table->size++;
        log(2, "Added to var_table \"%s\" variable", table->vars[table->size - 1].name);

        return 0;
}

int resize_table (table_t *table)
{
        assert_ptr(table);

        variable_t *temp = nullptr;
        if (table->size + 10 >= table->capacity) {
                temp = (variable_t*) realloc(table->vars, sizeof(variable_t) * table->capacity * 2);
                if (temp) {
                        table->vars = temp;
                } else {
                        log(1, "<span style = \"color: red; font-size:16px;\">!Realloc returned null!</span>");
                        return REALLOC_ERR;
                }
                log(1, "<span style = \"color: green; font-size:10px;\">Resized var_table</span>");
        }

        return 0;
}

int table_ctor (table_t *table, size_t capacity)
{
        assert_ptr(table);

        table->vars = (variable_t*) calloc(capacity, sizeof(variable_t));
        if (!table->vars) {
                log(1, "Calloc for variable table if NULL");
                return NULL_CALLOC;
        }

        table->capacity = capacity;

        return 0;
}
