#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <cstdlib>
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
        if (!node) {
                log_error(1, "Calloc returned NULL.");
                return nullptr;
        }
        node->atr      = temp_node->atr;
        node->data     = temp_node->data;
        node->type     = temp_node->type;
        node->sub_type = temp_node->sub_type;
        strcpy(node->name, temp_node->name);
        node->new_node = 1;
        node->left = temp_node->left;
        node->right = temp_node->right;

        return node;
}

int tree_dtor (tree_t *tree)
{
        if (!tree)
                return NULL_TREE_PTR;
        free_nodes(tree->root);

        return 0;
}

void free_nodes (node_t *node)
{
        assert(node);
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
