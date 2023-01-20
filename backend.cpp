#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree\tree.h"
#include "backend.h"

int create_asm (tree_t *tree, const char *file_name)
{
        assert_ptr(tree);

        table_t global_table = {};
        if (table_ctor(&global_table, 20, 20))
                return NULL_CALLOC;

        FILE *output = fopen(file_name, "w");

        log(1, "<span style = \"color: blue; font-size:30px;\">START WRITING ASSEMLER</span>");
        asm_node(tree->root, &global_table, output);

        fprintf(output, "hlt\n");
        table_dtor(&global_table);
        fclose(output);
        return 0;
}

#define push(num)               fprintf(output, "push %d %s", num, "\n")

#define rel_op(command,name)    log(2, "START Asm %s", #name);                                    \
                                fprintf(output, "%s %s\n", #command, #name);                      \
                                fprintf(output, "pop\n");                                         \
                                push(0);                                                          \
                                fprintf(output, "%s:\n", #name);                                  \
                                break;

int asm_node (node_t *node, table_t *gl_table, FILE *output)
{
        assert_ptr(node);
        assert_ptr(gl_table);

        if (node->left && node->type != AND  && node->type != OR &&
            node->type != CYCLE && node->type != RELATIVE_OP     &&
            node->type != FUNC_INIT) {
                asm_node(node->left, gl_table, output);
        }
        if (node->right && node->type != OPERATOR            &&
            node->type != AND  && node->type != OR           &&
            node->type != CYCLE && node->type != RELATIVE_OP &&
            node->type != FUNC_INIT && node->type != STAFF) {
                asm_node(node->right, gl_table, output);
        }

        size_t indent = 0;
        switch (node->type) {
        case FUNC:
                fprintf(output, "call %s\n", node->name);
                break;
        case STAFF:
                if (node->sub_type == RETURN) {
                        asm_node(node->right, gl_table, output);
                }
                break;
        case FUNC_INIT:
                func_init(output, node, gl_table);
                break;
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
                        rel_op(je, equal)
                case N_EQUAL:
                        rel_op(jne, n_equal)
                case GREATER:
                        rel_op(ja, greater)
                case SMALLER:
                        rel_op(jb, smaller)
                case ESMALLER:
                        rel_op(jbe, esmaller)
                case EGREATER:
                        rel_op(jae, egreater)
                default:
                        log(1, "<span style = \"color: red; font-size:16px;\">UNKOWN RELATIVE OPERATOR</span>");
                }
                break;
        case CYCLE:
                if (node->sub_type == WHILE) {
                        asm_while(output, gl_table, node);
                }
                break;
        case AND:
                asm_and(output, gl_table, node);
                break;
        case OR:
                asm_or(output, gl_table, node);
                break;
        case OPERATOR:
                asm_operator(output, gl_table, node);
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
                log(1, "Default switch case in assembling");
        }

        return 0;
}

#undef rel_op

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
        if (table->var_size + 10 >= table->var_cap) {
                temp = (variable_t*) realloc(table->vars, sizeof(variable_t) * table->var_cap * 2);
                if (temp) {
                        table->vars = temp;
                } else {
                        log(1, "<span style = \"color: red; font-size:16px;\">!Realloc returned null!</span>");
                        return REALLOC_ERR;
                }
                log(1, "<span style = \"color: green; font-size:10px;\">Resized var_table</span>");
        }

        function_t *temp2 = nullptr;
        if (table->func_size + 10 >= table->func_cap) {
                temp2 = (function_t*) realloc(table->funcs, sizeof(function_t) * table->func_cap * 2);
                if (temp2) {
                        table->funcs = temp2;
                } else {
                        log(1, "<span style = \"color: red; font-size:16px;\">!Realloc returned null!</span>");
                        return REALLOC_ERR;
                }
                log(1, "<span style = \"color: green; font-size:10px;\">Resized func_table</span>");
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

int table_dtor (table_t *table)
{
        assert_ptr(table);

        if (table->vars) {
                free(table->vars);
                table->vars = nullptr;
        } else {
                log(1, "<span style = \"color: orange; font-size:14px;\"> Destructor of table tried to free variables pointer </span>");
        }
        if (table->funcs) {
                free(table->funcs);
                table->funcs = nullptr;
        } else {
                log(1, "<span style = \"color: orange; font-size:14px;\"> Destructor of table tried to free functions pointer </span>");
        }

        return 0;
}

int asm_while (FILE *output, table_t *gl_table, node_t *node)
{
        assert_ptr(output);
        assert_ptr(gl_table);
        assert_ptr(node);

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

        return 0;
}

int asm_and (FILE *output, table_t *gl_table, node_t *node)
{
        assert_ptr(output);
        assert_ptr(gl_table);
        assert_ptr(node);
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
        return 0;
}

int asm_or (FILE *output, table_t *gl_table, node_t *node)
{
        assert_ptr(output);
        assert_ptr(gl_table);
        assert_ptr(node);
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
        return 0;
}

int asm_operator (FILE *output, table_t *gl_table, node_t *node)
{
        assert_ptr(output);
        assert_ptr(gl_table);
        assert_ptr(node);

        size_t indent = 0;

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

        return 0;
}

int func_init (FILE *output, node_t *node, table_t *table)
{
        assert_ptr(output);
        assert_ptr(node);
        assert_ptr(table);

        for (size_t i = 0; i < table->func_size; i++) {
                if (!strcmp(node->name, table->funcs[i].name)) {
                        log(2, "<span style = \"color: red; font-size:16px;\">!Function %s already initialized!</span>", node->name);
                        return INIT_ERROR;
                }
        }

        if (resize_table(table))
                return REALLOC_ERR;

        table->funcs[table->func_size].type = node->sub_type;
        strcpy(table->funcs[table->func_size].name, node->name);
        table->func_size++;
        log(2, "Added to func_table \"%s\" function", table->funcs[table->func_size - 1].name);

        asm_func(output, node, table);
        return 0;
}

int asm_func (FILE *output, node_t *node, table_t *table)
{
        assert_ptr(node);
        assert_ptr(table);

        resize_table(table);

        fprintf(output, "jmp skip%lld \n", table->func_size);
        fprintf(output, "%s:\n", node->name);

        asm_node(node->right, table, output);

        fprintf(output, "ret\n");
        fprintf(output, "skip%lld:\n", table->func_size);

        log(2, "Assmed \"%s\" function", node->name);
        return 0;
}

#undef push
