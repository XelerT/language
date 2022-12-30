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

        node_t *node = get_el(tokens, tp, tree);
        $s(node->name)
        assert(arg[*tp].type == END_FILE);
        return node;
}

node_t* get_el (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);

        node_t *l_node = get_e(tokens, tp, tree);
        node_t *node   = nullptr;

        while (arg[*tp].type == END_LINE) {
                $
                node_t temp_node  = {};
                token_arg_t token = {};
                copy_token(&token, arg + *tp);
                ++*tp;
                node_t *r_node = nullptr;
                if (arg[*tp].type != END_FILE) {
                        r_node = get_el(tokens, tp, tree);
                }
                edit_temp(&temp_node, &token);
                node = tree_insert(&temp_node);

                node->right = r_node;
                node->left  = l_node;
        }

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
        node_t *node   = nullptr;
        $s(l_node->name)

        while (arg[*tp].type == ADD_OPERATOR || arg[*tp].type == SUB_OPERATOR) {
                $
                $s(arg[*tp].name)
                node_t temp_node  = {};
                token_arg_t token = {};
                copy_token(&token, arg + *tp);
                ++*tp;
                node_t *r_node = get_e(tokens, tp, tree);

                edit_temp(&temp_node, &token);
                node = tree_insert(&temp_node);

                node->left  = l_node;
                node->right = r_node;
        }

        if (node)
                return node;
        $
        return l_node;
}

node_t* get_t (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);
        $
        node_t *l_node = get_if(tokens, tp, tree);
        node_t *node   = nullptr;
        while (arg[*tp].type == MUL_OPERATOR || arg[*tp].type == DIV_OPERATOR) {
                $d(arg[*tp].type)
                node_t temp_node  = {};
                token_arg_t token = {};
                copy_token(&token, arg + *tp);
                ++*tp;
                node_t *r_node = get_t(tokens, tp, tree);

                edit_temp(&temp_node, &token);
                node = tree_insert(&temp_node);

                node->left  = l_node;
                node->right = r_node;
        }

        if (node)
                return node;

        return l_node;
}

node_t* get_if (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);

        node_t *l_node = nullptr;
        node_t *line_node = nullptr;
        node_t *if_node   = nullptr;

        if (arg[*tp].type == OPERATOR && arg[*tp].sub_type == 2) {
                node_t temp_node = {
                        .type    = END_LINE,
                        .name = ";",
                };
                line_node = tree_insert(&temp_node);
                edit_temp(&temp_node, arg + *tp);
                if_node   = tree_insert(&temp_node);
                ++*tp;
                l_node = get_p(tokens, tp, tree);

                if_node->right  = get_e(tokens, tp, tree);
                line_node->left = if_node;
                if_node->left   =  l_node;
        } else {
                l_node = get_p(tokens, tp, tree);
        }

        if (line_node)
                return line_node;
        return l_node;
}

node_t* get_p (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);
        $
        node_t *node = nullptr;
        if (arg[*tp].type == OP_BRACKET) {
                ++*tp;
                node = get_e(tokens, tp, tree);

                assert(arg[*tp].type == CL_BRACKET);
                ++*tp;
        } else {
                node = get_a(tokens, tp, tree);
        }

        return node;
}

node_t* get_a (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);

        $
        node_t *l_node = get_n(tokens, tp, tree);
        $
        node_t *node   = nullptr;
        $
        $d(arg[*tp].type)
        while (arg[*tp].type == ASSIGNMENT) {
                node_t temp_node  = {};
                token_arg_t token = {};
                copy_token(&token, arg + *tp);
                ++*tp;
                node_t *r_node = get_e(tokens, tp, tree);

                edit_temp(&temp_node, &token);
                node = tree_insert(&temp_node);

                node->left  = l_node;
                node->right = r_node;
        }

        if (node)
                return node;

        return l_node;
}

node_t* get_n (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);

        size_t prev_tp = *tp;
        node_t node = {};
        log(2, "GET_N token type: %d", arg[*tp].type);
        switch (arg[*tp].type) {
        case NUMBER:
                node.data = arg[(*tp)++].val;
                node.type = NUMBER;
                break;
        case NAME:
                strcpy(node.name, arg[*tp].name);
                ++*tp;
                node.type = NAME;
                break;
        case DATA_TYPE:
                if (arg[*tp + 1].type == NAME) {
                        node.type = VARIABLE;
                        node.sub_type = arg[*tp].sub_type;
                        strcpy(node.name, arg[*tp + 1].name);
                        *tp += 2;
                        log(3, "Created variable with type: \"%d\", name: \"%s\"", node.sub_type, node.name);
                } else {
                        log(1, "<span style = \"color: red; font-size:16px;\">!No variable name after data type!</span>");
                }
                break;
        default:
                log(1, "Default case: token type: \"%d\", name: \"%s\"", arg[*tp].type, arg[*tp].name);
        }

        assert(*tp != prev_tp);
        return tree_insert(&node);
}

void edit_temp (node_t *temp_node, token_arg_t *token)
{
        assert_ptr(temp_node);
        assert_ptr(token);

        temp_node->data = token->val;
        strcpy(temp_node->name, token->name);
        temp_node->type = token->type;
        temp_node->atr = token->atr;
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
