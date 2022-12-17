#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "tree.h"

int tree_ctor (tree_t *tree)
{
        assert(tree);

        tree->root = (node_t*) calloc(1, sizeof(node_t));
        if (!tree->root)
                return NULL_CALLOC;
        tree->size = 1;
        tree->root->atr = {};

        return 0;
}

node_t* tree_insert (const node_t *temp_node)
{
        assert(temp_node);

        node_t *node = (node_t*) calloc(1, sizeof(node_t));
        if (!node)
                return nullptr;

        node->atr  = temp_node->atr;
        node->data = temp_node->data;
        node->type = temp_node->type;
        strcpy(node->func, temp_node->func);
        node->new_node = 1;
        node->left = nullptr;
        node->right = nullptr;

        return node;
}

int tree_dtor (tree_t *tree)
{
        if (!tree)
                return NULL_TREE_PTR;
        free_nodes(tree->root);
        // free(tree);

        return 0;
}

void free_nodes (node_t *node)
{
        assert(node);
        $p(node)
        $c(node->data.op)
        if (node->left) {
                free_nodes(node->left);
        }
        if (node->right) {
                free_nodes(node->right);
        }
        if (node->new_node) {
                free(node);
        }
}
