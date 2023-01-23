#ifndef TREE_2_TEXT
#define TREE_2_TEXT

#include "tree.h"

int write_node_data (FILE *text_tree, node_t *node);
int write_node      (FILE *text_tree, node_t *node, int n_tabs);
int tree_2_text     (tree_t *tree);
void print_tabs     (FILE *text_tree, int n_tabs);

#endif /*TREE_2_TEXT*/
