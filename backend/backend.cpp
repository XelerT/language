#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "..\tree\tree.h"
#include "backend.h"

int create_asm (tree_t *tree, const char *file_name)
{
        assert_ptr(tree);

        table_t global_table = {};
        if (table_ctor(&global_table, DEFAULT_N_VARS, DEFAULT_N_FUNCS))
                return NULL_CALLOC;

        FILE *output = fopen(file_name, "w");

        log(1, "<span style = \"color: blue; font-size:30px;\">START WRITING ASSEMLER</span>");
        base_asm_node(tree->root, &global_table, nullptr, output);

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

#define asm_node(node,gl_table,output) base_asm_node(node, gl_table, loc_table, output);
#define switch_table(arg) loc_table ? loc_table->arg: gl_table->arg
#define CMD(name,num,asm_code) else if (node->sub_type == name)                 \
                                        fprintf(output, "%s", asm_code);

#pragma GCC diagnostic ignored "-Wlogical-op"

int base_asm_node (node_t *node, table_t *gl_table, table_t *loc_table, FILE *output)
{
        assert_ptr(node);
        assert_ptr(gl_table);
        assert_ptr(output);

        if (node->left && node->type != AND  && node->type != OR     &&
            node->type != CYCLE && node->type != RELATIVE_OP         &&
            node->type != FUNC_INIT && node->type != FUNC            &&
            node->type != ADD_OPERATOR && node->type != SUB_OPERATOR &&
            node->type != MUL_OPERATOR && node->type != DIV_OPERATOR) {
                if (node->type != ASSIGNMENT || node->left->type != NAME)
                        asm_node(node->left, gl_table, output);
        }
        if (node->right && node->type != OPERATOR                    &&
            node->type != AND  && node->type != OR                   &&
            node->type != CYCLE && node->type != RELATIVE_OP         &&
            node->type != FUNC_INIT && node->type != STAFF           &&
            node->type != ADD_OPERATOR && node->type != SUB_OPERATOR &&
            node->type != MUL_OPERATOR && node->type != DIV_OPERATOR) {

                asm_node(node->right, gl_table, output);
        }

        size_t indent  = 0;
        size_t indent2 = 0;
        int is_casted  = 0;

        switch (node->type) {
        case FUNC:
                asm_node(node->left, gl_table, output);
                fprintf(output, "call %s\n", node->name);
                break;
        case STAFF:
                if (node->sub_type == RETURN) {
                        asm_node(node->right, gl_table, output);
                }
#include "..\include\special_cmds.cmds"
                break;
        case FUNC_INIT:
                func_init(output, node, gl_table);

                log(2, "Initialized \"%s\" function", node->name);
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
                asm_name(output, gl_table, node);
                break;
        case VARIABLE:
                if (var_init(gl_table, loc_table ? loc_table: nullptr, node->sub_type, node->name)) {
                        return INIT_ERROR;
                }
                log(2, "Initialized \"%s\" variable", node->name);
                break;
        case ASSIGNMENT:
                indent  = find_var(gl_table,  node->left->name);
                indent2 = find_var(gl_table,  node->right->name);
                $
                if (indent == SIZE_T_ERROR && loc_table) {
                        indent = find_var(loc_table, node->left->name);
                }
                if (indent2 == SIZE_T_ERROR && loc_table) {
                        indent2 = find_var(loc_table, node->left->name);
                }
                if (indent == SIZE_T_ERROR) {
                        log(2, "<span style = \"color: red; font-size:16px;\">!Variable %s was not initialized!</span>", node->left->name);
                        return INIT_ERROR;
                }
                $
                // if (node->left->type == NAME && node->type == ASSIGNMENT)
                //         fprintf(output, "push [%s + %lld]\npop\n", indent != SIZE_T_ERROR ? "rax": "rbx", indent);

                if (switch_table(vars[indent].type) == INT && ((node->right->sub_type == FLOAT || node->right->casted) ||
                                                                (indent2 != SIZE_T_ERROR && (switch_table(vars[indent2].type) == FLOAT              ||
                                                                gl_table->vars[indent2].type == FLOAT)))) {
                        $
                        fprintf(output, "push 100\n");
                        fprintf(output, "div\n");
                } else if (switch_table(vars[indent].type) == FLOAT && ((node->right->sub_type == INT && !node->right->casted) ||
                                                                        (indent2 != SIZE_T_ERROR && (switch_table(vars[indent2].type) == INT              ||
                                                                        gl_table->vars[indent2].type == INT)))) {
                        $
                        fprintf(output, "push 100\n");
                        fprintf(output, "mul\n");
                }
$
                if (switch_table(vars[indent].type) == INT || gl_table->vars[indent].type == INT)
                        fprintf(output, "pop [%s + %lld]\n", loc_table ? "rbx": "rax", indent);
                else if (switch_table(vars[indent].type) == FLOAT || gl_table->vars[indent].type == INT)
                        fprintf(output, "popf [%s + %lld]\n", loc_table ? "rbx": "rax", indent);

                log(2, "Assigned to rax + %d", indent);
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
                is_casted = cast_type(node, gl_table, nullptr);
                if (is_casted < 0)
                        node->left->casted = is_casted;
                else if (is_casted > 0)
                        node->right->casted = is_casted;

                asm_node(node->left, gl_table, output);
                asm_node(node->right, gl_table, output);

                fprintf(output, "add\n");

                log(1, "add");
                break;
        case SUB_OPERATOR:
                is_casted = cast_type(node, gl_table, nullptr);
                if (is_casted < 0)
                        node->left->casted = is_casted;
                else if (is_casted > 0)
                        node->right->casted = is_casted;

                asm_node(node->left, gl_table, output);
                asm_node(node->right, gl_table, output);

                fprintf(output, "sub\n");

                log(1, "sub");
                break;
        case MUL_OPERATOR:
                is_casted = cast_type(node, gl_table, nullptr);
                if (is_casted < 0)
                        node->left->casted = is_casted;
                else if (is_casted > 0)
                        node->right->casted = is_casted;

                asm_node(node->left, gl_table, output);
                asm_node(node->right, gl_table, output);

                fprintf(output, "mul\n");

                log(1, "mul");
                break;
        case DIV_OPERATOR:
                is_casted = cast_type(node, gl_table, nullptr);
                if (is_casted < 0)
                        node->left->casted = is_casted;
                else if (is_casted > 0)
                        node->right->casted = is_casted;

                asm_node(node->left, gl_table, output);
                asm_node(node->right, gl_table, output);
                cast_type(node, gl_table, nullptr);

                fprintf(output, "div\n");

                log(1, "div");
                break;
        default:
                log(2, "Default switch case in assembling(%s)", node->name);
        }

        return 0;
}

#undef CMD
#undef rel_op
#undef asm_node
#undef switch_table

#define switch_table(arg) loc_table.vars ? loc_table.arg: gl_table->arg
#define asm_node(node,gl_table,output) base_asm_node(node, gl_table, loc_table.vars ? &loc_table: nullptr, output);

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
                        log(2, "<span style = \"color: red; font-size:16px;\">!Variable %s already initialized as global variable!</span>", name);
                        return INIT_ERROR;
                }
                if (resize_table(gl_table))
                        return REALLOC_ERR;
        }
        if (loc_table) {
                for (size_t i = 0; i < loc_table->var_size; i++) {
                        if (!strcmp(name, loc_table->vars[i].name)) {
                                log(2, "<span style = \"color: red; font-size:16px;\">!Variable %s already initialized as local variable!</span>", name);
                                return INIT_ERROR;
                        }
                }
                if (resize_table(loc_table)) {
                        return REALLOC_ERR;
                }
                loc_table->vars[loc_table->var_size].val  = 0;
                loc_table->vars[loc_table->var_size].type = (char) sub_type;                ////////////////////////////////////TODO
                strcpy(loc_table->vars[loc_table->var_size].name, name);
                loc_table->var_size++;
                log(2, "Added to local var_table \"%s\" variable", loc_table->vars[loc_table->var_size - 1].name);
        } else {
                gl_table->vars[gl_table->var_size].val  = 0;
                gl_table->vars[gl_table->var_size].type = (char) sub_type;
                strcpy(gl_table->vars[gl_table->var_size].name, name);
                gl_table->var_size++;
                log(2, "Added to global var_table \"%s\" variable", gl_table->vars[gl_table->var_size - 1].name);
        }

        return 0;
}

int asm_while (FILE *output, table_t *gl_table, node_t *node)
{
        assert_ptr(output);
        assert_ptr(gl_table);
        assert_ptr(node);

        log(1, "START Asm WHILE");

        table_t loc_table = {};
        if (count_args(node, gl_table)) {
                table_ctor(&loc_table, DEFAULT_N_VARS, 0);
        }
        fprintf(output, "while_%lld:\n", switch_table(while_size));
        if (node->left) {
               asm_node(node->left, gl_table, output);
        }
        push(0);
        fprintf(output, "je end_while_%lld\n", switch_table(while_size));
        if (node->right) {
                asm_node(node->right, gl_table, output);
        }
        fprintf(output, "jmp while_%lld\n",  switch_table(while_size));
        fprintf(output, "end_while_%lld:\n", switch_table(while_size++));

        log(1, "END Asm WHILE");
        table_dtor(&loc_table);
        return 0;
}

int asm_and (FILE *output, table_t *gl_table, node_t *node)
{
        assert_ptr(output);
        assert_ptr(gl_table);
        assert_ptr(node);

        log(1, "START Asm AND");

        table_t loc_table = {};
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

        table_t loc_table = {};
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
        table_t loc_table = {};
        if (count_args(node, gl_table))
                table_ctor(&loc_table, DEFAULT_N_VARS, DEFAULT_N_FUNCS);

        if (node->sub_type == IF) {
                log(1, "START Asm IF");
                if (node->right->type     == OPERATOR &&
                    node->right->sub_type == ELSE) {
                        log(1, "START Asm ELSE");

                        indent = switch_table(if_size);
                        push(0);
                        fprintf(output, "je end_if_%lld\n", switch_table(if_size++));
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
                        indent = switch_table(if_size);
                        push(0);
                        fprintf(output, "je end_if_%lld\n", switch_table(if_size++));
                        if (node->right) {
                                asm_node(node->right, gl_table, output);
                        }
                        fprintf(output, "end_if_%lld:\n", indent);
                }
                log(1, "END Asm IF");
        }

        table_dtor(&loc_table);
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

        table_t loc_table = {};
        table_ctor(&loc_table, DEFAULT_N_VARS, 0);
        loc_table.var_size = count_args(node->left, &loc_table);

        fprintf(output, "\njmp skip%lld \n", table->func_size);
        fprintf(output, "%s:\n", node->name);

        asm_node(node->left, table, output);

        for (size_t i = loc_table.var_size - 1; i > 0; i--) {
                if (loc_table.vars[i].type == INT)
                        fprintf(output, "pop [rbx + %lld]\n", i);
                else if (loc_table.vars[i].type == FLOAT)
                        fprintf(output, "popf [rbx + %lld]\n", i);

                log(2, "Add argument with \"%s\" name", loc_table.vars[i].name);
        }

        asm_node(node->right, table, output);

        fprintf(output, "ret\n");
        fprintf(output, "skip%lld:\n\n", table->func_size);

        log(2, "Assmed \"%s\" function", node->name);
        table_dtor(&loc_table);
        return 0;
}

int asm_name (FILE *output, table_t *gl_table, node_t *node)
{
        assert_ptr(output);
        assert_ptr(gl_table);
        assert_ptr(node);

        log(1, "START Asm NAME");
        size_t indent = find_var(gl_table, node->name);
        if (indent == SIZE_T_ERROR) {
                log(2, "<span style = \"color: red; font-size:16px;\">!Variable %s was not initialized!</span>", node->name);
                return INIT_ERROR;
        }

        if (gl_table->vars[indent].type == INT) {
                if (node->casted) {
                        fprintf(output, "push 100\n");
                        fprintf(output, "pushf [rax + %lld]\n", indent);
                        fprintf(output, "mul\n");
                } else {
                        fprintf(output, "push [rax + %lld]\n", indent);
                }
        } else if (gl_table->vars[indent].type == FLOAT) {
                fprintf(output, "push [rax + %lld]\n", indent);
        }

        log(1, "END Asm NAME");
        return 0;
}

size_t count_args(node_t *node, table_t *table)
{
        assert_ptr(node);
        assert_ptr(table);

        size_t count = 0;

        if (node->left)
                count += count_args(node->left,  table);
        if (node->right)
                count += count_args(node->right, table);

        if (node->type == VARIABLE) {
                count++;
        $s(node->name)
        }

        return count;
}

#undef push
#undef asm_node

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
                        log(1, "<span style = \"color: red; font-size:16px;\">!Realloc returned null!</span>");
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
                        log(1, "<span style = \"color: red; font-size:16px;\">!Realloc returned null!</span>");
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

#undef switch_table

#define switch_table(arg) loc_table ? loc_table->arg: gl_table->arg

int cast_type (node_t *node, table_t *gl_table, table_t *loc_table)
{
        assert_ptr(node);
        assert_ptr(gl_table);

        if (node->right && node->left) {
                size_t indent1 = find_var(gl_table, node->left->name);
                if (!indent1 && loc_table)
                        indent1 = find_var(loc_table, node->left->name);

                size_t indent2 = 0;
                $d(node->right->type)
                if (node->right->type != NAME && node->right->type != NUMBER) {

                        indent2 = find_var(gl_table, node->right->left->name);
                        if (!indent2 && loc_table)
                                indent2 = find_var(loc_table, node->right->left->name);
                } else {
                $
                        indent2 = find_var(gl_table, node->right->name);
                        $lld(indent2)
                        $p(loc_table)
                        if (indent2 == SIZE_T_ERROR && loc_table)
                                indent2 = find_var(loc_table, node->right->name);
                }
                $d(node->right->sub_type)
                if (node->left->type == NAME && node->right->type == NAME) {
                        if (switch_table(vars[indent1].type) == INT && switch_table(vars[indent2].type) == FLOAT)
                                return -1;
                        if (switch_table(vars[indent1].type) == FLOAT && switch_table(vars[indent2].type) == INT)
                                return 1;
                } else if (node->left->type == NAME && node->right->type == NUMBER) {
                        if (switch_table(vars[indent1].type) == INT && node->right->sub_type == FLOAT)
                                return -1;
                        if (switch_table(vars[indent1].type) == FLOAT && node->right->sub_type == INT)
                                return 1;
                } else if (node->left->type == NUMBER && node->right->type == NAME) {
                        if (node->left->sub_type == INT && switch_table(vars[indent2].type) == FLOAT)
                                return -1;
                        if (node->left->sub_type == FLOAT && switch_table(vars[indent2].type) == INT)
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

#undef switch_tables
