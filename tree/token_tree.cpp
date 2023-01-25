#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>


#include "..\include\lexer.h"
#include "..\include\config.h"
#include "token_tree.h"

#define arg tokens->tok_args

node_t* get_g (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);
        log(1, "<span style = \"color: blue; font-size:30px;\">START GETTING TREE</span>");

        node_t *node = get_el(tokens, tp, tree);

        assert(arg[*tp].type == END_FILE);
        return node;
}

node_t* get_el (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);

        node_t *l_node = nullptr;
        if (arg[*tp].type != END_LINE)
                l_node = get_e(tokens, tp, tree);
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
                        r_node = get_el(tokens, tp, tree);
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

node_t* get_e (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);

        node_t *l_node = get_t(tokens, tp, tree);
        log(2, "Type after If %s", l_node->name);

        node_t *node   = nullptr;

        while (arg[*tp].type == ADD_OPERATOR || arg[*tp].type == SUB_OPERATOR) {
                node_t temp_node  = {};
                token_arg_t token = {};
                copy_token(&token, arg + *tp);
                ++*tp;
                node_t *r_node = get_e(tokens, tp, tree);

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

node_t* get_t (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);

        node_t *l_node = get_if(tokens, tp, tree);
        log(2, "Type after If %s", l_node->name);
        node_t *node   = nullptr;
        if (arg[*tp].type == MUL_OPERATOR || arg[*tp].type == DIV_OPERATOR) {
                node_t temp_node  = {};
                token_arg_t token = {};
                copy_token(&token, arg + *tp);
                ++*tp;
                node_t *r_node = get_t(tokens, tp, tree);

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
        node_t *if_node   = nullptr;

        if ((arg[*tp].type == OPERATOR && arg[*tp].sub_type == IF) ||
            (arg[*tp].type == CYCLE    && arg[*tp].sub_type == WHILE)) {
                node_t temp_node = {};
                edit_temp(&temp_node, arg + *tp);
                if_node = tree_insert(&temp_node);
                ++*tp;
                l_node = get_p(tokens, tp, tree);
                log(3, "Token type in if: %d %s", arg[*tp].type, arg[*tp].name);
                node_t *r_node = get_p(tokens, tp, tree);

                if (arg[*tp].type == OPERATOR && arg[*tp].sub_type == ELSE) {
                        if_node->right = get_else(tokens, tp, tree);
                        if_node->right->left = r_node;
                } else {
                        if_node->right = r_node;
                }
                if_node->left =  l_node;
                if_node->atr.fillcolor = "#18D5F5";
        } else {
                l_node = get_wh(tokens, tp, tree);
        }

        log(2, "Type after If %d", arg[*tp].type);
        if (if_node)
                return if_node;
        return l_node;
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
        else_node->right = get_p(tokens, tp, tree);
        else_node->atr.fillcolor = "#F51E2A";

        if (else_node)
                return else_node;
        return l_node;
}

node_t* get_wh (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);

        node_t *l_node    = nullptr;
        node_t *while_node   = nullptr;

        if (arg[*tp].type == CYCLE && arg[*tp].sub_type == WHILE) {
                node_t temp_node = {};
                edit_temp(&temp_node, arg + *tp);
                while_node = tree_insert(&temp_node);
                ++*tp;
                l_node = get_p(tokens, tp, tree);
                log(3, "Token type in while: %d %s", arg[*tp].type, arg[*tp].name);
                node_t *r_node = get_el(tokens, tp, tree);

                while_node->right = r_node;
                while_node->left  =  l_node;
                while_node->atr.fillcolor = "#00d8ff";
        } else {
                l_node = get_conj(tokens, tp, tree);
        }

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

        node_t *l_node = get_p(tokens, tp, tree);
        node_t *node   = nullptr;
        node_t temp_node = {};

        if (arg[*tp].type == AND || arg[*tp].type == OR) {
                log(2, "Created node with conjanction %s", arg[*tp].name);
                edit_temp(&temp_node, arg + *tp);
                ++*tp;
                node        = tree_insert(&temp_node);
                node->left  = l_node;
                node->right = get_p(tokens, tp, tree);
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
                        log(1, "<span style = \"color: red; font-size:16px;\">!Unknown relative operator!</span>");
                }
                ++*tp;
                node        = tree_insert(&temp_node);
                node->left  = l_node;
                node->right = get_p(tokens, tp, tree);
                node->atr.fillcolor = "#98B1B5";
        }

        if (node)
                return node;
        return l_node;
}

node_t* get_p (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);

        node_t *node = nullptr;

        if (arg[*tp].type == OP_BRACKET) {
                ++*tp;
                node = get_e(tokens, tp, tree);
                log(2, "Type in e: %d", arg[*tp].type);
                assert(arg[*tp].type == CL_BRACKET);
                ++*tp;
        } else {
                node = get_ne(tokens, tp, tree);
        }

        log(2, "Type after P %d", arg[*tp].type);
        return node;
}

node_t* get_ne (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);

        node_t *l_node = get_a(tokens, tp, tree);
        node_t *node   = nullptr;

        if (arg[*tp].type == NEXT_ELEM) {
                node_t temp_node = {};
                edit_temp(&temp_node, arg + *tp);
                ++*tp;
                node = tree_insert(&temp_node);
                node->right = get_e(tokens, tp, tree);
                log(2, "In NEXT_ELEM after E: %d", arg[*tp].type);
                node->left = l_node;
        }

        if (node)
                return node;
        return l_node;
}


node_t* get_a (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);

        node_t *l_node = get_cb(tokens, tp, tree);
        node_t *node   = nullptr;

        while (arg[*tp].type == ASSIGNMENT) {
                log(2, "Type in A %d", arg[*tp].type);
                node_t temp_node  = {};
                token_arg_t token = {};
                copy_token(&token, arg + *tp);
                ++*tp;
                node_t *r_node = get_e(tokens, tp, tree);

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

node_t* get_cb (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);

        node_t *node = nullptr;

        if (arg[*tp].type == OP_C_BRACKET) {
                ++*tp;
                log(2, "Type after { %d", arg[*tp].type);
                node = get_el(tokens, tp, tree);
                log(3, "Type in cb: %d name: %s", arg[*tp].type, arg[*tp].name);
                assert(arg[*tp].type == CL_C_BRACKET);
                ++*tp;
        } else {
                node = get_func(tokens, tp, tree);
        }

        log(2, "Type after cb %d", arg[*tp].type);
        return node;
}

node_t* get_func (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);

        node_t *node   = nullptr;

        if (arg[*tp].type     == DATA_TYPE && arg[*tp + 1].type == NAME &&
            arg[*tp + 2].type == OP_BRACKET) {
                        node_t temp_node = {};
                        temp_node.sub_type = arg[*tp].sub_type;
                        strcpy(temp_node.name, arg[*tp + 1].name);
                        temp_node.type = FUNC_INIT;
                        *tp += 2;
                        log(3, "Created function with type: \"%d\", name: \"%s\"", temp_node.sub_type, temp_node.name);
                        temp_node.atr.fillcolor = "#EB8CD5";

                        node = tree_insert(&temp_node);
                        if (arg[*tp + 1].type != CL_BRACKET)
                                node->left  = get_p(tokens, tp, tree);
                        else
                                *tp += 2;
                        node->right = get_cb(tokens, tp, tree);
        } else if (arg[*tp].type == NAME && arg[*tp + 1].type == OP_BRACKET) {
                node_t temp_node = {};

                edit_temp(&temp_node, arg + *tp);
                temp_node.type = FUNC;
                ++*tp;
                log(2, "Function %s called", temp_node.name);
                temp_node.atr.fillcolor = "#D681C2";

                node        = tree_insert(&temp_node);
                node->left  = get_p (tokens, tp, tree);
        } else {
                node        = get_n(tokens, tp, tree);
        }

        log(2, "Type after function %d", arg[*tp].type);
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
                                log(1, "<span style = \"color: red; font-size:16px;\">!No variable name after data type!</span>");
                                recycle = 0;
                        }
                        break;
                case STAFF:
                        node.type = STAFF;
#include "..\include\special_cmds.cmds"
                        /*else*/ if (arg[*tp].sub_type == RETURN) {
                                        node.sub_type = RETURN;
                                        strcpy(node.name, "return");
                                        log(1, "Created return");
                        } else {
                                log(1, "Unknown standart command");
                        }
                        ++*tp;
                        node.left = get_e(tokens, tp, tree);
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
