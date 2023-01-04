#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree\tree.h"
#include "backend.h"

int create_asm (tree_t *tree, const char *file_name)
{
        assert_ptr(tree);

        table_t global_table = {};
        if (table_ctor(&global_table, 20))
                return NULL_CALLOC;

        FILE *output = fopen(file_name, "w");

        log(1, "<span style = \"color: blue; font-size:30px;\">START GETTING ASSEMLER</span>");
        asm_node(tree->root, &global_table, output);

        fprintf(output, "hlt\n");
        fclose(output);
        return 0;
}

#define push(num) fprintf(output, "push %d %s", num, "\n")

int asm_node (node_t *node, table_t *gl_table, FILE *output)
{
        assert_ptr(node);
        assert_ptr(gl_table);

        if (node->left && node->type != AND  && node->type != OR &&
            node->type != CYCLE && node->type != RELATIVE_OP) {
                asm_node(node->left, gl_table, output);
        }
        if (node->right && node->type != OPERATOR  &&
            node->type != AND  && node->type != OR &&
            node->type != CYCLE && node->type != RELATIVE_OP) {
                asm_node(node->right, gl_table, output);
        }

        size_t indent = 0;
        switch (node->type) {
        case RELATIVE_OP:
                push(1);
                if (node->left) {
                        asm_node(node->left, gl_table, output);
                }
                if (node->right) {
                        asm_node(node->right, gl_table, output);
                }
                switch (node->sub_type) {
                case EQUAL:
                        log(1, "START Asm EQUAL");
                        fprintf(output, "je equal\n");
                        fprintf(output, "pop\n");
                        push(0);
                        fprintf(output, "equal:\n");
                        break;
                case N_EQUAL:
                        log(1, "START Asm N_EQUAL");
                        fprintf(output, "jne n_equal\n");
                        fprintf(output, "pop\n");
                        push(0);
                        fprintf(output, "n_equal:\n");
                        break;
                case GREATER:
                        log(1, "START Asm GREATER");
                        fprintf(output, "ja greater\n");
                        fprintf(output, "pop\n");
                        push(0);
                        fprintf(output, "greater:\n");
                        break;
                case SMALLER:
                        log(1, "START Asm SMALLER");
                        fprintf(output, "jb smaller\n");
                        fprintf(output, "pop\n");
                        push(0);
                        fprintf(output, "smaller:\n");
                        break;
                case ESMALLER:
                        log(1, "START Asm ESMALLER");
                        fprintf(output, "jbe esmaller\n");
                        fprintf(output, "pop\n");
                        push(0);
                        fprintf(output, "esmaller:\n");
                        break;
                case EGREATER:
                        log(1, "START Asm EGREATER");
                        fprintf(output, "jae egreater\n");
                        fprintf(output, "pop\n");
                        push(0);
                        fprintf(output, "egreater:\n");
                        break;
                }
        case CYCLE:
                if (node->sub_type == WHILE) {
                        log(1, "START Asm WHILE");
                        fprintf(output, "while_%lld:\n", gl_table->while_size);
                        if (node->left) {
                                asm_node(node->left, gl_table, output);
                        }
                        push(0);
                        fprintf(output, "je end_while_%lld\n", gl_table->while_size);
                        if (node->right) {
                                asm_node(node->right, gl_table, output);
                        }
                        fprintf(output, "jmp while_%lld\n", gl_table->while_size);
                        fprintf(output, "end_while_%lld:\n", gl_table->while_size++);
                        log(1, "END Asm WHILE");
                }
                break;
        case AND:
                log(1, "START Asm AND");
                push(0);
                if (node->left) {
                        asm_node(node->left, gl_table, output);
                }
                push(0);
                fprintf(output, "je cond_and_1_%lld\n", gl_table->and_size);
                if (node->right) {
                        asm_node(node->right, gl_table, output);
                }
                push(0);
                fprintf(output, "je cond_and_2_%lld\n", gl_table->and_size);
                fprintf(output, "pop\n");
                fprintf(output, "push 1\n");
                fprintf(output, "cond_and_1_%lld:\n", gl_table->and_size);
                fprintf(output, "cond_and_2_%lld:\n", gl_table->and_size++);
                log(1, "END Asm AND");
                break;
        case OR:
                log(1, "START Asm OR");
                push(1);
                if (node->left) {
                        asm_node(node->left, gl_table, output);
                }
                push(0);
                fprintf(output, "jne cond_or_%lld\n", gl_table->or_size);
                if (node->right) {
                        asm_node(node->right, gl_table, output);
                }
                push(0);
                fprintf(output, "jne cond_or_%lld\n", gl_table->or_size);
                fprintf(output, "pop\n");
                fprintf(output, "push 0\n");
                fprintf(output, "cond_or_%lld:\n", gl_table->or_size++);
                log(1, "END Asm OR");
                break;
        case OPERATOR:
                if (node->sub_type == IF) {
                        log(1, "START Asm IF");
                        if (node->right->type     == OPERATOR &&
                            node->right->sub_type == ELSE) {
                                log(1, "START Asm ELSE");
                                indent = gl_table->if_size;
                                push(0);
                                fprintf(output, "je end_if_%lld\n", gl_table->if_size++);
                                if (node->right->left) {
                                        asm_node(node->right->left, gl_table, output);
                                }
                                fprintf(output, "jmp end_else_%lld\n", indent);
                                fprintf(output, "end_if_%lld:\n", indent);
                                if (node->right->right) {
                                        asm_node(node->right->right, gl_table, output);
                                        fprintf(output, "end_else_%lld:\n", indent);
                                }
                                log(1, "END Asm ELSE");
                        } else {
                                indent = gl_table->if_size;
                                push(0);
                                fprintf(output, "je end_if_%lld\n", gl_table->if_size++);
                                if (node->right) {
                                        asm_node(node->right, gl_table, output);
                                }
                                fprintf(output, "end_if_%lld:\n", indent);
                        }
                        log(1, "END Asm IF");
                }
                break;
        case NAME:
                log(1, "START Asm NAME");
                indent = find_var(gl_table, node->name);
                if (indent == SIZE_T_ERROR) {
                        log(2, "<span style = \"color: red; font-size:16px;\">!Variable %s was not initialized!</span>", node->name);
                        return INIT_ERROR;
                }
                fprintf(output, "push [rax + %lld]\n", indent);
                log(1, "END Asm NAME");
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

        for (size_t i = 0; i < table->var_size; i++) {
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

        for (size_t i = 0; i < table->var_size; i++) {
                if (!strcmp(name, table->vars[i].name)) {
                        log(2, "<span style = \"color: red; font-size:16px;\">!Variable %s already initialized!</span>", name);
                        return INIT_ERROR;
                }
        }

        if (resize_table(table))
                return REALLOC_ERR;

        table->vars[table->var_size].val = 0;
        strcpy(table->vars[table->var_size].name, name);
        table->var_size++;
        log(2, "Added to var_table \"%s\" variable", table->vars[table->var_size - 1].name);

        return 0;
}

int resize_table (table_t *table)
{
        assert_ptr(table);

        variable_t *temp = nullptr;
        if (table->var_size + 10 >= table->var_capacity) {
                temp = (variable_t*) realloc(table->vars, sizeof(variable_t) * table->var_capacity * 2);
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

        table->var_capacity = capacity;

        return 0;
}
