#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>


#include "../include/lexer.h"
#include "../include/config.h"
#include "token_tree.h"
#include "../include/errors_handler.h"

#define arg tokens->tok_args

#define check(node) if (!node)                  \
                        return nullptr;

node_t* get_g (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);
        log(1, "<span style = \"color: blue; font-size:30px;\">START GETTING TREE</span>");

        node_t *node = get_end_line(tokens, tp, tree);
        if (!node) {
                tree_dtor(tree);
                return nullptr;
        } else if (check_type(tokens, tp, END_FILE, "Unexpected symbol", 1)) {
                tree_dtor(tree);
                return nullptr;
        }

        assert(arg[*tp].type == END_FILE);
        return node;
}

node_t* get_end_line(const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);

        node_t *l_node = nullptr;
        if (arg[*tp].type != END_LINE) {
                l_node = hub(tokens, tp, tree);
                check(l_node)
        }
        node_t *node   = nullptr;
        node_t temp_node  = {};
        token_arg_t token = {
                .type = END_LINE,
                .name = ";"
        };
        node_t *r_node = nullptr;
        log(2, "Type in el %d", arg[*tp].type);

        while (arg[*tp].type == END_LINE || (arg[*tp - 1].type == CL_C_BRACKET && arg[*tp].type != END_FILE
                                                                               && arg[*tp].type != CL_C_BRACKET)) {
                log(3, "**Token type in EL: %d %s (prev elem: %s)", arg[*tp].type, arg[*tp].name, arg[*tp - 1].name);
                if (arg[*tp].type == END_LINE)
                        ++*tp;
                if (arg[*tp].type != END_FILE && arg[*tp].type != CL_C_BRACKET) {
                        r_node = get_end_line(tokens, tp, tree);
                        check(r_node);
                        log(2, "Type after El2 %d", arg[*tp].type);
                }
                edit_temp(&temp_node, &token);
                node = tree_insert(&temp_node);
                node->atr.fillcolor = "#BDCCF2";

                node->right = r_node;
                node->left  = l_node;
        }
        log(2, "Type after El %d", arg[*tp].type);

        if (node)
                return node;

        return l_node;
}

node_t* hub (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);

        node_t *node = nullptr;
        log(1, "IN HUB");

        if (arg[*tp].type != END_FILE) {
                if (arg[*tp].type == DATA_TYPE) {
                        if (arg[*tp + 1].type == NAME) {
                                if (arg[*tp + 2].type == OP_BRACKET) {
                                        node = get_func_init(tokens, tp, tree);
                                        check(node);
                                } else {
                                        node = get_div_mul(tokens, tp, tree);
                                        check(node);
                                        log(1, "After e in hub");
                                }
                        } else {
                                if (check_type(tokens, tp + 1, NAME, "No variable/function name after data type", 1))
                                        return nullptr;
                        }
                } else if (arg[*tp].type == NAME) {
                        if (arg[*tp + 1].type == OP_BRACKET) {
                                node = get_func(tokens, tp, tree);
                                check(node);
                        } else {
                                node = get_div_mul(tokens, tp, tree);
                                check(node);
                        }
                } else if (arg[*tp].type == CYCLE) {
                        if (arg[*tp].type == WHILE) {
                                node = get_while(tokens, tp, tree);
                                check(node);
                        } else
                                log_error(1, "Unknown cycle");
                } else if (arg[*tp].type == OPERATOR && arg[*tp].sub_type == IF) {
                        node = get_if(tokens, tp, tree);
                        check(node);
                } else if (arg[*tp].type == STAFF) {
                        node = get_div_mul(tokens, tp, tree);
                        check(node);
                } else {
                        log_error(1, "errrrrrorrrrrrrrrr");
                }
        }
        return node;
}

node_t* get_div_mul (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);

        node_t *l_node = get_sub_add(tokens, tp, tree);
        check(l_node);
        log(2, "Type after t %s", l_node->name);

        node_t *node   = nullptr;

        while (arg[*tp].type == ADD_OPERATOR || arg[*tp].type == SUB_OPERATOR) {
                node_t temp_node  = {};
                token_arg_t token = {};
                copy_token(&token, arg + *tp);
                ++*tp;
                node_t *r_node = get_div_mul(tokens, tp, tree);
                check(r_node);

                edit_temp(&temp_node, &token);
                node = tree_insert(&temp_node);
                node->atr.fillcolor = "#F5B5D9";

                node->left  = l_node;
                node->right = r_node;
        }
        log(2, "Type after E %d", arg[*tp].type);
        if (node)
                return node;
        return l_node;
}

node_t* get_sub_add(const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);

        node_t *l_node = get_conj(tokens, tp, tree);
        check(l_node);
        log(2, "Type after If %s", l_node->name);
        node_t *node   = nullptr;
        if (arg[*tp].type == MUL_OPERATOR || arg[*tp].type == DIV_OPERATOR) {
                node_t temp_node  = {};
                token_arg_t token = {};
                copy_token(&token, arg + *tp);
                ++*tp;
                node_t *r_node = get_sub_add(tokens, tp, tree);
                check(r_node);

                edit_temp(&temp_node, &token);
                node = tree_insert(&temp_node);
                node->atr.fillcolor = "#F5B5D9";

                node->left  = l_node;
                node->right = r_node;
        }
        log(2, "Type after T %d", arg[*tp].type);
        if (node)
                return node;
        return l_node;
}

node_t* get_if (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);

        node_t *l_node    = nullptr;

        node_t temp_node = {};
        edit_temp(&temp_node, arg + *tp);
        node_t *if_node = tree_insert(&temp_node);
        ++*tp;
        l_node = get_brackets(tokens, tp, tree);
        check(l_node);
        log(3, "Token type in if: %d %s", arg[*tp].type, arg[*tp].name);
        node_t *r_node = get_brackets(tokens, tp, tree);
        check(r_node);

        if (arg[*tp].type == OPERATOR && arg[*tp].sub_type == ELSE) {
                if_node->right = get_else(tokens, tp, tree);
                check(if_node->right);
                if_node->right->left = r_node;
        } else {
                if_node->right = r_node;
        }
        if_node->left = l_node;
        if_node->atr.fillcolor = "#18D5F5";

        log(2, "Type after If %d", arg[*tp].type);

        return if_node;
}

node_t* get_else (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);

        node_t *l_node    = nullptr;
        node_t *else_node = nullptr;

        log(3, "Token type in else: %d %s", arg[*tp].type, arg[*tp].name);
        node_t temp_node = {};
        edit_temp(&temp_node, arg + *tp);
        ++*tp;
        else_node = tree_insert(&temp_node);
        else_node->right = get_brackets(tokens, tp, tree);
        check(else_node->right)
        else_node->atr.fillcolor = "#F51E2A";

        if (else_node)
                return else_node;
        return l_node;
}

node_t* get_while (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);

        node_t *l_node     = nullptr;
        node_t *while_node = nullptr;

        node_t temp_node = {};
        edit_temp(&temp_node, arg + *tp);
        while_node = tree_insert(&temp_node);
        ++*tp;
        l_node = get_brackets(tokens, tp, tree);
        check(l_node)
        log(3, "Token type in while: %d %s", arg[*tp].type, arg[*tp].name);

        while_node->right = get_curv_brack(tokens, tp, tree);
        check(while_node->right)
        while_node->left  =  l_node;
        while_node->atr.fillcolor = "#00d8ff";

        log(2, "Type after If %d", arg[*tp].type);
        if (while_node)
                return while_node;
        return l_node;
}

node_t* get_conj (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);

        node_t *l_node   = get_brackets(tokens, tp, tree);
        check(l_node)
        node_t *node     = nullptr;
        node_t temp_node = {};

        if (arg[*tp].type == AND || arg[*tp].type == OR) {
                log(2, "Created node with conjanction %s", arg[*tp].name);
                edit_temp(&temp_node, arg + *tp);
                ++*tp;
                node = tree_insert(&temp_node);

                node->left  = l_node;
                node->right = get_brackets(tokens, tp, tree);
                check(node->right)
                node->atr.fillcolor = "#98B1B5";
        } else if (arg[*tp].type == RELATIVE_OP) {
                edit_temp(&temp_node, arg + *tp);
                switch (arg[*tp].name[0]) {
                case '=':
                        temp_node.sub_type = EQUAL;
                        break;
                case '!':
                        temp_node.sub_type = N_EQUAL;
                        break;
                case '>':
                        if (arg[*tp].name[1] == '\0')
                                temp_node.sub_type = GREATER;
                        else
                              temp_node.sub_type = EGREATER;
                        break;
                case '<':
                        if (arg[*tp].name[1] == '\0')
                                temp_node.sub_type = SMALLER;
                        else
                              temp_node.sub_type = ESMALLER;
                        break;
                default:
                        log_error(1, "!Unknown relative operator!");
                }
                ++*tp;
                node        = tree_insert(&temp_node);
                node->left  = l_node;

                node->right = get_brackets(tokens, tp, tree);
                check(node->right)
                node->atr.fillcolor = "#98B1B5";
        }

        if (node)
                return node;
        return l_node;
}

node_t* get_brackets (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);

        node_t *node = nullptr;

        if (arg[*tp].type == OP_BRACKET) {
                ++*tp;
                node = get_div_mul(tokens, tp, tree);
                check(node)
                log(2, "Type after e: %d", arg[*tp].type);

                if (check_type(tokens, tp, CL_BRACKET, "There are expected )", 1))
                        return nullptr;
                assert(arg[*tp].type == CL_BRACKET);
                ++*tp;
        } else {
                node = get_next_elem(tokens, tp, tree);
                check(node)
        }

        log(2, "Type after P %d", arg[*tp].type);
        return node;
}

node_t* get_next_elem (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);

        node_t *l_node = get_assign(tokens, tp, tree);
        check(l_node)
        node_t *node   = nullptr;

        if (arg[*tp].type == NEXT_ELEM) {
                node_t temp_node = {};
                edit_temp(&temp_node, arg + *tp);
                ++*tp;
                node = tree_insert(&temp_node);

                if (check_arg(tokens, tp))
                        return nullptr;
                node->right = get_div_mul(tokens, tp, tree);
                check(node->right)
                log(2, "In NEXT_ELEM after E: %d", arg[*tp].type);
                node->left = l_node;
        }

        if (node)
                return node;
        return l_node;
}

node_t* get_assign(const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);

        node_t *l_node = get_curv_brack(tokens, tp, tree);
        check(l_node)
        node_t *node   = nullptr;

        while (arg[*tp].type == ASSIGNMENT) {
                log(2, "Type in A %d", arg[*tp].type);
                node_t temp_node  = {};
                token_arg_t token = {};
                copy_token(&token, arg + *tp);
                ++*tp;

                node_t *r_node = get_div_mul(tokens, tp, tree);
                check(r_node)

                edit_temp(&temp_node, &token);
                node = tree_insert(&temp_node);
                node->atr.fillcolor = "#CED8DB";

                node->left  = l_node;
                node->right = r_node;
        }

        log(2, "Type after A %d", arg[*tp].type);
        if (node)
                return node;

        return l_node;
}

node_t* get_curv_brack (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);

        node_t *node = nullptr;

        if (arg[*tp].type == OP_C_BRACKET) {
                ++*tp;
                log(2, "Type after { %d", arg[*tp].type);
                node = get_end_line(tokens, tp, tree);
                check(node)
                log(3, "Type in cb: %d name: %s", arg[*tp].type, arg[*tp].name);

                if (check_type(tokens, tp, CL_C_BRACKET, "There are expected }", 1))
                        return nullptr;
                assert(arg[*tp].type == CL_C_BRACKET);
                ++*tp;
        } else {
                node = get_func(tokens, tp, tree);
                check(node)
        }

        log(2, "Type after cb %d", arg[*tp].type);
        return node;
}

node_t* get_func_init (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);

        node_t *node     = nullptr;
        node_t temp_node =      {};

        temp_node.sub_type = arg[*tp].sub_type;
        strcpy(temp_node.name, arg[*tp + 1].name);
        temp_node.type = FUNC_INIT;

        *tp += 2;
        temp_node.atr.fillcolor = "#EB8CD5";
        node = tree_insert(&temp_node);
        log(3, "Created function with type: \"%d\", name: \"%s\"", temp_node.sub_type, temp_node.name);

        if (arg[*tp + 1].type != CL_BRACKET) {
                node->left = get_brackets(tokens, tp, tree);
                check(node->left)
        } else
                *tp += 2;
        log(1, "after p in func init");
        node->right = get_curv_brack(tokens, tp, tree);
        check(node->right)

        log(2, "Type after function %d", arg[*tp].type);
        return node;
}

node_t *get_func (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);

        node_t *node     = nullptr;
        node_t temp_node =      {};

        if (arg[*tp].type == NAME && arg[*tp + 1].type == OP_BRACKET) {
                edit_temp(&temp_node, arg + *tp);
                temp_node.type = FUNC;
                ++*tp;
                log(2, "Function %s called", temp_node.name);
                temp_node.atr.fillcolor = "#D681C2";

                node = tree_insert(&temp_node);
                if (arg[*tp + 1].type != CL_BRACKET) {
                        node->left  = get_brackets(tokens, tp, tree);
                        check(node->left)
                } else
                        *tp += 2;
        } else {
                node = get_n(tokens, tp, tree);
                check(node)
        }

        return node;
}

#define CMD(cmd_name, ...)      if (arg[*tp].sub_type == cmd_name) {                    \
                                        node.sub_type = cmd_name;                       \
                                        strcpy(node.name, #cmd_name);                   \
                                        log(2, "Created %s", #cmd_name);                \
                                } else                                                  \

node_t* get_n (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);

        size_t prev_tp = *tp;
        node_t node = {};
        int recycle = -1;
        log(2, "GET_N token type: %d", arg[*tp].type);

        while (recycle) {
                recycle = 1;
                switch (arg[*tp].type) {
                case NUMBER:
                        node.data = arg[*tp].val;
                        node.type = NUMBER;
                        node.sub_type = arg[*tp].sub_type;
                        node.atr.fillcolor = "#CEEFF5";
                        ++*tp;
                        break;
                case NAME:
                        strcpy(node.name, arg[*tp].name);
                        node.type = NAME;
                        ++*tp;
                        break;
                case DATA_TYPE:
                        if (arg[*tp + 1].type == NAME) {
                                node.sub_type = arg[*tp].sub_type;
                                strcpy(node.name, arg[*tp + 1].name);
                                node.type = VARIABLE;
                                *tp += 2;
                                log(3, "Created variable with type: \"%d\", name: \"%s\"", node.sub_type, node.name);
                                node.atr.fillcolor = "#93F558";
                        } else {
                                if (check_type(tokens, tp + 1, NAME, "No variable name after data type", 1))
                                        return nullptr;
                                log_error(1, "!No variable name after data type!");
                                recycle = 0;
                        }
                        break;
                case STAFF:
                        node.type = STAFF;
#include "../include/special_cmds.cmds"
                        /*else*/ if (arg[*tp].sub_type == RETURN) {
                                        node.sub_type = RETURN;
                                        strcpy(node.name, "return");
                                        log(1, "Created return");
                        } else {
                                log(1, "Unknown standart command");
                        }
                        ++*tp;
                        if (check_type(tokens, tp, NAME, "No argument after standart function", 0)) {
                                if (check_type(tokens, tp, NUMBER, "No argument after standart function", 0))
                                        if (check_type(tokens, tp, FUNC, "No argument after standart function", 1))
                                                return nullptr;
                        }


                        node.left = get_div_mul(tokens, tp, tree);
                        break;
                default:
                        log(3, "Default case: token type: \"%d\", name: \"%s\"", arg[*tp].type, arg[*tp].name);
                        assert(recycle == 1);
                        recycle = 0;
                }
        }

        log(2, "Type after N %d", arg[*tp].type);
        assert(*tp != prev_tp);
        return tree_insert(&node);
}

void edit_temp (node_t *temp_node, token_arg_t *token)
{
        assert_ptr(temp_node);
        assert_ptr(token);

        temp_node->data     = token->val;
        strcpy(temp_node->name, token->name);
        temp_node->type     = token->type;
        temp_node->sub_type = token->sub_type;
        temp_node->atr      = token->atr;
}

void copy_token (token_arg_t *dest_token, token_arg_t *src_token)
{
        assert_ptr(src_token);
        assert_ptr(dest_token);

        dest_token->type = src_token->type;
        strcpy(dest_token->name, src_token->name);
        dest_token->val = src_token->val;
        dest_token->atr = src_token->atr;
}
