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

        node_t *node = get_e(tokens, tp, tree);
        $s(node->name)
        assert(arg[*tp].type == END_FILE);
        return node;
}

node_t* get_e (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);

        node_t *l_node = get_t(tokens, tp, tree);
        node_t *node   = nullptr;

        $s(l_node->name)

        while (arg[*tp].type == PM_OPERATOR) {
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

        node_t *l_node = get_p(tokens, tp, tree);
        node_t *node   = nullptr;

        while(arg[*tp].type == MD_OPERATOR) {
                node_t temp_node  = {};
                token_arg_t token = {};
                copy_token(arg + *tp, &token);
                ++*tp;
                node_t *r_node = get_t(tokens, tp, tree);

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

node_t* get_p (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);

        node_t *node = nullptr;
        if (arg[*tp].type == OP_BRACKET) {
                ++*tp;
                node = get_e(tokens, tp, tree);

                assert(arg[*tp].type == CL_BRACKET);
                ++*tp;
        } else {
                $
                node = get_n(tokens, tp, tree);
        }

        return node;
}

node_t* get_n (const tokens_t *tokens, size_t *tp, tree_t *tree)
{
        assert_ptr(tokens);
        assert_ptr(tp);
        assert_ptr(tree);

        elem_t val = 0;
        size_t prev_tp = *tp;
        node_t node = {};

        if (arg[*tp].type == NUMBER) {
                val = arg[(*tp)++].val;
        } else if (arg[*tp].type == VARIABLE) {
                $
                strcpy(node.name, arg[*tp].name);
                ++*tp;
        }
        $

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

void copy_token(token_arg_t *dest_token, token_arg_t *src_token)
{
        assert_ptr(src_token);
        assert_ptr(dest_token);

        dest_token->type = src_token->type;
        strcpy(dest_token->name, src_token->name);
        dest_token->val = src_token->val;
        dest_token->atr = src_token->atr;
}
