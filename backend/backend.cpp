#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../tree/tree.h"
#include "backend.h"

int create_asm (tree_t *tree, const char *file_name)
{
        assert_ptr(tree);

        tab_table_t table = {};
        table_t gl_table = {};
        table.gl_table = &gl_table;
        if (tab_table_ctor(&table, DEFAULT_N_TABLS))
                return NULL_CALLOC;

        FILE *output = fopen(file_name, "w");

        log(1, "<span style = \"color: blue; font-size:30px;\">START WRITING ASSEMLER</span>");
        fprintf(output, "push 100\n");
        fprintf(output, "pop rbx\n");
        asm_node(tree->root, &table, output);

        fprintf(output, "hlt\n");
        tab_table_dtor(&table);
        fclose(output);
        return 0;
}

#define push(num)               fprintf(output, "push %d %s", num, "\n")

#define rel_op(command,name,num)    log(2, "START Asm %s", #name);                                                   \
                                    fprintf(output, "%s %s%ld\n", #command, #name, num);                            \
                                    fprintf(output, "pop\n");                                                        \
                                    push(0);                                                                         \
                                    fprintf(output, "%s%ld:\n", #name, num);                                        \
                                    break;

#pragma GCC diagnostic ignored "-Wlogical-op"

int asm_node (node_t *node, tab_table_t *table, FILE *output)
{
        assert_ptr(node);
        assert_ptr(table);
        assert_ptr(output);

        table_t *gl_table  = table->gl_table;

        if (node->left && node->type != AND  && node->type != OR     &&
            node->type != CYCLE && node->type != RELATIVE_OP         &&
            node->type != FUNC_INIT && node->type != FUNC            &&
            node->type != ADD_OPERATOR && node->type != SUB_OPERATOR &&
            node->type != MUL_OPERATOR && node->type != DIV_OPERATOR &&
            node->type != STAFF) {
                if (node->type != ASSIGNMENT || node->left->type != NAME)
                        asm_node(node->left, table, output);
        }

        if (node->right && node->type != OPERATOR                    &&
            node->type != AND  && node->type != OR                   &&
            node->type != CYCLE && node->type != RELATIVE_OP         &&
            node->type != FUNC_INIT && node->type != STAFF           &&
            node->type != ADD_OPERATOR && node->type != SUB_OPERATOR &&
            node->type != MUL_OPERATOR && node->type != DIV_OPERATOR) {

                asm_node(node->right, table, output);
        }

        int is_casted  = 0;
        int error      = 0;

        switch (node->type) {
        case FUNC:
                asm_node(node->left, table, output);
                fprintf(output, "call %s\n", node->name);
                break;
        case STAFF:
                asm_staff(output, node, table);
                break;
        case FUNC_INIT:
                func_init(output, node, table);

                log(2, "Initialized \"%s\" function", node->name);
                break;
        case RELATIVE_OP:
                push(1);
                if (node->left) {
                        asm_node(node->left, table, output);
                }
                if (node->right) {
                        asm_node(node->right, table, output);
                }
                switch (node->sub_type) {
                case EQUAL:
                        rel_op(je, equal, table->gl_table->rel_op_size)
                case N_EQUAL:
                        rel_op(jne, n_equal, table->gl_table->rel_op_size)
                case GREATER:
                        rel_op(jb, greater, table->gl_table->rel_op_size)
                case SMALLER:
                        rel_op(ja, smaller, table->gl_table->rel_op_size)
                case ESMALLER:
                        rel_op(jbe, esmaller, table->gl_table->rel_op_size)
                case EGREATER:
                        rel_op(jae, egreater, table->gl_table->rel_op_size)
                default:
                        log_error(1, "UNKOWN RELATIVE OPERATOR");
                }
                table->gl_table->rel_op_size++;
                break;
        case CYCLE:
                if (node->sub_type == WHILE) {
                        asm_while(output, table, node);
                }
                break;
        case AND:
                asm_and(output, table, node);
                break;
        case OR:
                asm_or(output, table, node);
                break;
        case OPERATOR:
                asm_operator(output, table, node);
                break;
        case NAME:
                asm_name(output, table, node);
                break;
        case VARIABLE:
                if (var_init(gl_table, table->loc_size > 0 ? table->loc_tables[table->loc_size - 1]: nullptr, node->sub_type, node->name)) {
                        return INIT_ERROR;
                }
                log(2, "Initialized \"%s\" variable", node->name);
                break;
        case ASSIGNMENT:
                error = asm_assignment(output, node, table);
                if (error)
                        return error;
                break;
        case NUMBER:
                fprintf(output, "push %d\n", node->data);
                if (node->casted) {
                        fprintf(output, "push 100\n");
                        fprintf(output, "mul\n");
                }

                log(2, "Pushed %d", node->data);
                break;
        case ADD_OPERATOR:
                is_casted = cast_type(node, table);
                if (is_casted < 0)
                        node->left->casted = is_casted;
                else if (is_casted > 0)
                        node->right->casted = is_casted;

                asm_node(node->left, table, output);
                asm_node(node->right, table, output);

                fprintf(output, "add\n");

                log(1, "add");
                break;
        case SUB_OPERATOR:
                is_casted = cast_type(node, table);
                if (is_casted < 0)
                        node->left->casted = is_casted;
                else if (is_casted > 0)
                        node->right->casted = is_casted;

                asm_node(node->left, table, output);
                asm_node(node->right, table, output);

                fprintf(output, "sub\n");

                log(1, "sub");
                break;
        case MUL_OPERATOR:
                is_casted = cast_type(node, table);
                if (is_casted < 0)
                        node->left->casted = is_casted;
                else if (is_casted > 0)
                        node->right->casted = is_casted;

                asm_node(node->left, table, output);
                asm_node(node->right, table, output);

                fprintf(output, "mul\n");

                log(1, "mul");
                break;
        case DIV_OPERATOR:
                is_casted = cast_type(node, table);
                if (is_casted < 0)
                        node->left->casted = is_casted;
                else if (is_casted > 0)
                        node->right->casted = is_casted;

                asm_node(node->left, table, output);
                asm_node(node->right, table, output);
                cast_type(node, table);

                fprintf(output, "div\n");

                log(1, "div");
                break;
        default:
                log(2, "Default switch case in assembling(%s, type:%ld)", node->name, node->type);
        }
        return 0;
}

#undef CMD
#undef rel_op
#undef asm_name

int indent_rbx (FILE *output, size_t n_args)
{
        assert_ptr(output);

        fprintf(output, "push rbx\n");
        fprintf(output, "pop rdx\n");
        fprintf(output, "push rbx\n");
        fprintf(output, "push %ld\n", n_args);
        fprintf(output, "add\n");
        fprintf(output, "pop rbx\n");
        // fprintf(output, "push rdx\n");

        log(2, "Indent rbx on %ld", n_args);

        return 0;
}

int asm_return (FILE *output, tab_table_t *table)
{
        assert_ptr(output);

        fprintf(output, "pop  rdx\n");
        fprintf(output, "pop  rbx\n");
        fprintf(output, "push rdx\n");
        fprintf(output, "ret\n");

        table->gl_table->funcs[table->gl_table->func_size].has_return = 1;

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

int var_init (table_t *gl_table, table_t *loc_table, int sub_type, char *name)
{
        assert_ptr(gl_table);
        assert_ptr(name);

        for (size_t i = 0; i < gl_table->var_size; i++) {
                if (!strcmp(name, gl_table->vars[i].name)) {
                        log_error(2, "!Variable %s already initialized as global variable!", name);
                        return INIT_ERROR;
                }
                if (resize_table(gl_table))
                        return REALLOC_ERR;
        }

        if (loc_table) {
                for (size_t i = 0; i < loc_table->var_size; i++) {
                        if (!strcmp(name, loc_table->vars[i].name)) {
                                log_error(2, "!Variable %s already initialized as local variable!", name);
                                return INIT_ERROR;
                        }
                }
                if (resize_table(loc_table)) {
                        return REALLOC_ERR;
                }

                loc_table->vars[loc_table->var_size].val  = 0;
                loc_table->vars[loc_table->var_size].type = sub_type;
                strcpy(loc_table->vars[loc_table->var_size].name, name);
                loc_table->var_size++;

                log(2, "Added to local var_table \"%s\" variable", loc_table->vars[loc_table->var_size - 1].name);
        } else {
                gl_table->vars[gl_table->var_size].val  = 0;
                gl_table->vars[gl_table->var_size].type = sub_type;
                strcpy(gl_table->vars[gl_table->var_size].name, name);
                gl_table->var_size++;

                log(2, "Added to global var_table \"%s\" variable", gl_table->vars[gl_table->var_size - 1].name);
        }

        return 0;
}

int asm_while (FILE *output, tab_table_t *table, node_t *node)
{
        assert_ptr(output);
        assert_ptr(table);
        assert_ptr(node);

        log(1, "START Asm WHILE");

        table_t loc_table = {};
        table_t *gl_table = table->gl_table;

        if (count_args(node)) {
                table_ctor(&loc_table, DEFAULT_N_VARS, 0);

                if (resize_tab_table(table))
                        return REALLOC_ERR;
                table->loc_tables[table->loc_size++] = &loc_table;
        }
        fprintf(output, "while_%ld:\n", gl_table->while_size);
        if (node->left) {
               asm_node(node->left, table, output);
        }
        push(0);
        fprintf(output, "je end_while_%ld\n", gl_table->while_size);
        if (node->right) {
                asm_node(node->right, table, output);
        }
        fprintf(output, "jmp while_%ld\n",  gl_table->while_size);
        fprintf(output, "end_while_%ld:\n", gl_table->while_size++);

        log(1, "END Asm WHILE");
        table_dtor(&loc_table);
        table->loc_tables[table->loc_size--] = nullptr;

        return 0;
}

int asm_and (FILE *output, tab_table_t *table, node_t *node)
{
        assert_ptr(output);
        assert_ptr(table);
        assert_ptr(node);

        log(1, "START Asm AND");

        table_t *gl_table = table->gl_table;

        push(0);
        if (node->left) {
                asm_node(node->left, table, output);
        }
        push(0);
        fprintf(output, "je cond_and_1_%ld\n", gl_table->and_size);
        if (node->right) {
                asm_node(node->right, table, output);
        }
        push(0);
        fprintf(output, "je cond_and_2_%ld\n", gl_table->and_size);
        fprintf(output, "pop\n");
        fprintf(output, "push 1\n");
        fprintf(output, "cond_and_1_%ld:\n", gl_table->and_size);
        fprintf(output, "cond_and_2_%ld:\n", gl_table->and_size++);

        log(1, "END Asm AND");
        return 0;
}

int asm_or (FILE *output, tab_table_t *table, node_t *node)
{
        assert_ptr(output);
        assert_ptr(table);
        assert_ptr(node);

        log(1, "START Asm OR");

        table_t *gl_table = table->gl_table;

        push(1);
        if (node->left) {
                asm_node(node->left, table, output);
        }
        push(0);
        fprintf(output, "jne cond_or_%ld\n", gl_table->or_size);
        if (node->right) {
                asm_node(node->right, table, output);
        }
        push(0);
        fprintf(output, "jne cond_or_%ld\n", gl_table->or_size);
        fprintf(output, "pop\n");
        fprintf(output, "push 0\n");
        fprintf(output, "cond_or_%ld:\n", gl_table->or_size++);

        log(1, "END Asm OR");
        return 0;
}

int asm_operator (FILE *output, tab_table_t *table, node_t *node)
{
        assert_ptr(output);
        assert_ptr(table);
        assert_ptr(node);

        size_t indent = 0;
        table_t *gl_table = table->gl_table;
        table_t loc_table = {};
        if (count_args(node)) {
                table_ctor(&loc_table, DEFAULT_N_VARS, DEFAULT_N_FUNCS);

                if (resize_tab_table(table))
                        return REALLOC_ERR;
                table->loc_tables[table->loc_size++] = &loc_table;
        }

        if (node->sub_type == IF) {
                log(1, "START Asm IF");
                if (node->right->type     == OPERATOR &&
                    node->right->sub_type == ELSE) {
                        log(1, "START Asm ELSE");

                        indent = gl_table->if_size;
                        push(0);
                        fprintf(output, "je end_if_%ld\n", gl_table->if_size++);
                        if (node->right->left) {
                                asm_node(node->right->left, table, output);
                        }
                        fprintf(output, "jmp end_else_%ld\n", indent);
                        fprintf(output, "end_if_%ld:\n", indent);
                        if (node->right->right) {
                                asm_node(node->right->right, table, output);
                                fprintf(output, "end_else_%ld:\n", indent);
                        }
                        log(1, "END Asm ELSE");
                } else {
                        indent = gl_table->if_size;
                        push(0);
                        fprintf(output, "je end_if_%ld\n", gl_table->if_size++);
                        if (node->right) {
                                asm_node(node->right, table, output);
                        }
                        fprintf(output, "end_if_%ld:\n", indent);
                }
                log(1, "END Asm IF");
        }

        table_dtor(&loc_table);
        return 0;
}

int func_init (FILE *output, node_t *node, tab_table_t *table)
{
        assert_ptr(output);
        assert_ptr(node);
        assert_ptr(table);

        table_t *gl_table = table->gl_table;

        for (size_t i = 0; i < gl_table->func_size; i++) {
                if (!strcmp(node->name, gl_table->funcs[i].name)) {
                        log_error(2, "!Function %s already initialized!", node->name);
                        return INIT_ERROR;
                }
        }
        if (resize_table(gl_table))
                return REALLOC_ERR;
        gl_table->funcs[gl_table->func_size].type = node->sub_type;
        strcpy(gl_table->funcs[gl_table->func_size].name, node->name);
        gl_table->func_size++;

        log(2, "Added to func_table \"%s\" function", gl_table->funcs[gl_table->func_size - 1].name);

        asm_func(output, node, table);

        return 0;
}

size_t count_all_vars (tab_table_t *table)
{
        assert_ptr(table);

        size_t n_vars = table->gl_table->var_size;
        for (size_t i = 0; i < table->loc_size; i++) {
                n_vars += table->loc_tables[i]->var_size;
        }

        return n_vars;
}

int asm_func (FILE *output, node_t *node, tab_table_t *table)
{
        assert_ptr(node);
        assert_ptr(table);

        table_t *gl_table = table->gl_table;
        resize_table(gl_table);
        int is_main = 1;

        if (strcmp(node->name, "main")) {
                fprintf(output, "\njmp skip%ld \n", gl_table->func_size);
                is_main = 0;
        }
        fprintf(output, "%s:\n", node->name);

        table_t loc_table = {};

        size_t n_args = count_args(node);

        if (n_args) {
                table_ctor(&loc_table, DEFAULT_N_VARS, 0);

                if (resize_tab_table(table))
                        return REALLOC_ERR;
                table->loc_tables[table->loc_size++] = &loc_table;
                if (node->left)
                        loc_table.var_size = count_args(node->left);
        }

        if (node->left) {
                log(1, "here");
                asm_node(node->left, table, output);
        }

        if (loc_table.var_cap && !is_main)
                indent_rbx(output, n_args + count_all_vars(table));

        if (loc_table.var_size != 0) {
                for (size_t i = loc_table.var_size - 1; i > 0 && !is_main; i--) {
                        if (loc_table.vars[i].type == INT)
                                fprintf(output, "pop [rbx + %ld]\n", i);
                        else if (loc_table.vars[i].type == FLOAT)
                                fprintf(output, "popf [rbx + %ld]\n", i);

                        log(2, "Add argument with \"%s\" name", loc_table.vars[i].name);
                }
                fprintf(output, "push rdx\n");
        }

        loc_table.var_size = n_args - loc_table.var_size;

        log(3, "here %s %ld", node->name, node->right->type);

        if (node->right)
                asm_node(node->right, table, output);
        else
                log(1, "NULLPTR");

        if (!gl_table->funcs[gl_table->func_size].has_return && !is_main) {
                asm_return(output, table);
        }

        if (!is_main) {
                fprintf(output, "skip%ld:\n\n", gl_table->func_size++);
        }

        log(2, "Assmed \"%s\" function", node->name);

        return 0;
}

int asm_name (FILE *output, tab_table_t *table, node_t *node)
{
        assert_ptr(output);
        assert_ptr(table);
        assert_ptr(node);

        log(1, "START Asm NAME");
        size_t indent = find_var(table->gl_table, node->name);
        int in_loc_table = 0;
        size_t loc_t_indent = 0;
        table_t *gl_table = table->gl_table;

        if (indent == SIZE_T_ERROR && table->loc_size > 0) {
                for (size_t i = 0; i < table->loc_size && indent == SIZE_T_ERROR; i++) {
                        indent = find_var(table->loc_tables[i], node->name);
                        loc_t_indent++;
                }
                in_loc_table = 1;
        }
        if (indent == SIZE_T_ERROR) {
                log_error(2, "!Variable %s was not initialized!", node->name);
                return INIT_ERROR;
        }

        if (in_loc_table) {
                if (table->loc_tables[loc_t_indent - 1]->vars[indent].type == INT) {
                        if (node->casted) {
                                fprintf(output, "push 100\n");
                                fprintf(output, "pushf [rbx + %ld]\n", indent);
                                fprintf(output, "mul\n");
                        } else {
                                fprintf(output, "push [rbx + %ld]\n", indent);
                        }
                } else if (table->loc_tables[loc_t_indent - 1]->vars[indent].type == FLOAT) {
                        fprintf(output, "push [rbx + %ld]\n", indent);
                }
        } else {
                if (gl_table->vars[indent].type == INT) {
                        if (node->casted) {
                                fprintf(output, "push 100\n");
                                fprintf(output, "pushf [rax + %ld]\n", indent);
                                fprintf(output, "mul\n");
                        } else {
                                fprintf(output, "push [rax + %ld]\n", indent);
                        }
                } else if (gl_table->vars[indent].type == FLOAT) {
                        fprintf(output, "push [rax + %ld]\n", indent);
                }
        }

        log(1, "END Asm NAME");
        return 0;
}

size_t count_args(node_t *node)
{
        assert_ptr(node);

        size_t count = 0;

        if (node->left)
                count += count_args(node->left);
        if (node->right)
                count += count_args(node->right);

        if (node->type == VARIABLE) {
                count++;
        }

        return count;
}

#undef push
#undef asm_node

int cast_type (node_t *node, tab_table_t *table)
{
        assert_ptr(node);
        assert_ptr(table);

        table_t *gl_table = table->gl_table;
        size_t n_loc_table1 = 0;
        size_t n_loc_table2 = 0;

        if (node->right && node->left) {
                int first_in_loc_table = 0;
                size_t indent1 = find_var(gl_table, node->left->name);
                if (indent1 == SIZE_T_ERROR) {
                        for (size_t i = 0; i < table->loc_size && indent1 == SIZE_T_ERROR; i++) {
                                indent1 = find_var(table->loc_tables[i], node->left->name);
                                first_in_loc_table++;
                                n_loc_table1++;
                        }
                }
                int second_in_loc_table = 0;
                size_t indent2 = 0;

                if (node->right->type != NAME && node->right->type != NUMBER) {
                        indent2 = find_var(gl_table, node->right->left->name);
                        if (indent2 == SIZE_T_ERROR) {
                                for (size_t i = 0; i < table->loc_size && indent2 == SIZE_T_ERROR; i++) {
                                        indent2 = find_var(table->loc_tables[i], node->right->left->name);
                                        n_loc_table2++;
                                }
                        second_in_loc_table = 1;
                        }
                } else {
                        indent2 = find_var(gl_table, node->right->name);
                        if (indent2 == SIZE_T_ERROR) {
                                for (size_t i = 0; i < table->loc_size && indent2 == SIZE_T_ERROR; i++) {
                                        indent2 = find_var(table->loc_tables[i], node->right->name);
                                        n_loc_table2++;
                                }
                        second_in_loc_table = 1;
                        }
                }
                log(3, "Indent1: %ld, Indent2: %ld", indent1, indent2);

                int first_var_type = 0;
                int second_var_type = 0;

                if (indent1 != SIZE_T_ERROR) {
                        if (first_in_loc_table)
                                first_var_type = table->loc_tables[n_loc_table1 - 1]->vars[indent1].type;
                        else
                                first_var_type =  gl_table->vars[indent1].type;
                }
                if (indent2 != SIZE_T_ERROR) {
                        if (second_in_loc_table)
                                second_var_type = table->loc_tables[n_loc_table2 - 1]->vars[indent2].type;
                        else
                                second_var_type =  gl_table->vars[indent2].type;
                }

                if (node->left->type == NAME && node->right->type == NAME) {
                        if (first_var_type == INT && second_var_type == FLOAT)
                                return -1;
                        if (first_var_type == FLOAT && second_var_type == INT)
                                return 1;
                } else if (node->left->type == NAME && node->right->type == NUMBER) {
                        if (first_var_type == INT && node->right->sub_type == FLOAT)
                                return -1;
                        if (first_var_type == FLOAT && node->right->sub_type == INT)
                                return 1;
                } else if (node->left->type == NUMBER && node->right->type == NAME) {
                        if (node->left->sub_type == INT && second_var_type  == FLOAT)
                                return -1;
                        if (node->left->sub_type == FLOAT && second_var_type == INT)
                                return 1;
                } else if (node->left->type == NUMBER && node->right->type == NUMBER) {
                        if (node->left->sub_type == INT && node->right->sub_type == FLOAT)
                                return -1;
                        if (node->left->sub_type == FLOAT && node->right->sub_type == INT)
                                return 1;
                }
        }

        return 0;
}

int asm_assignment (FILE *output, node_t *node, tab_table_t *table)
{
        assert_ptr(output);
        assert_ptr(node);
        assert_ptr(table);

        int first_in_loc_table       = 0;
        int sec_in_loc_table         = 0;
        unsigned int first_var_type  = 0;
        unsigned int second_var_type = 0;
        size_t n_loc_table1          = 0;
        size_t n_loc_table2          = 0;

        table_t *gl_table = table->gl_table;

        size_t indent  = find_var(gl_table,  node->left->name);
        size_t indent2 = find_var(gl_table,  node->right->name);

        if (indent == SIZE_T_ERROR) {
                for (size_t i = 0; i < table->loc_size && indent == SIZE_T_ERROR; i++) {
                        indent = find_var(table->loc_tables[i], node->left->name);
                        first_in_loc_table++;
                        n_loc_table1++;
                }
        }
        if (indent2 == SIZE_T_ERROR) {
                for (size_t i = 0; i < table->loc_size && indent2 == SIZE_T_ERROR; i++) {
                        indent2 = find_var(table->loc_tables[i], node->right->name);
                        n_loc_table2++;
                        sec_in_loc_table++;
                }
        }
        if (indent == SIZE_T_ERROR) {
                log_error(2, "!Variable %s was not initialized!", node->left->name);
                return INIT_ERROR;
        }

        if (first_in_loc_table) {
                first_var_type = table->loc_tables[n_loc_table1 - 1]->vars[indent].type;
        } else {
                first_var_type = gl_table->vars[indent].type;
        }

        if (indent2 != SIZE_T_ERROR) {
                if (sec_in_loc_table) {
                        second_var_type = table->loc_tables[n_loc_table2 - 1]->vars[indent2].type;
                } else {
                        second_var_type = gl_table->vars[indent2].type;
                }
        }

        if (first_var_type == INT && (node->right->sub_type == FLOAT || node->right->casted ||
                                     (indent2 != SIZE_T_ERROR && second_var_type == FLOAT))) {
                fprintf(output, "push 100\n");
                fprintf(output, "div\n");
        } else if (first_var_type == FLOAT && ((node->right->sub_type == INT && !node->right->casted) ||
                                               (indent2 != SIZE_T_ERROR && (second_var_type == INT)))) {
                fprintf(output, "push 100\n");
                fprintf(output, "mul\n");
        }
        if (first_var_type == INT)
                fprintf(output, "pop [%s + %ld]\n", first_in_loc_table ? "rbx": "rax", indent);
        else if (first_var_type == FLOAT)
                fprintf(output, "popf [%s + %ld]\n", first_in_loc_table ? "rbx": "rax", indent);

        log(2, "Assigned to rax + %d", indent);

        return 0;
}

int asm_scan (FILE *output, node_t *node, tab_table_t *table)
{
        assert_ptr(node);
        assert_ptr(table);
        assert_ptr(output);

        fprintf(output, "in\n");

        if (node->type == NUMBER) {
                log_error(1, "Tried to scan in number");
                return LEX_ERROR;
        } else if (node->type == NAME) {
                size_t indent = find_var(table->gl_table, node->name);
                int in_loc_table = 0;

                if (indent == SIZE_T_ERROR && table->loc_size > 0) {
                        for (size_t i = 0; i < table->loc_size && indent == SIZE_T_ERROR; i++) {
                                indent = find_var(table->loc_tables[i], node->name);
                        }
                        in_loc_table = 1;
                }
                fprintf(output, "pop [%s + %ld]\n", in_loc_table ? "rbx": "rax", indent);
        }

        return 0;
}

int asm_staff (FILE *output, node_t *node, tab_table_t *table)
{
        assert_ptr(node);
        assert_ptr(table);
        assert_ptr(output);

        if (node->sub_type == RETURN) {
                asm_node(node->left, table, output);
                asm_return(output, table);
        } else if (node->sub_type == SCAN) {
                asm_scan(output, node->left, table);
        } else if (node->sub_type == PRINT) {
                asm_node(node->left, table, output);
                fprintf(output, "out\n");
        } else if (node->sub_type == SQRT) {
                asm_node(node->left, table, output);
                fprintf(output, "sqrt\n");
        }

        return 0;
}
