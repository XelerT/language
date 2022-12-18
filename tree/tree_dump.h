#ifndef TREE_DUMP_H
#define TREE_DUMP_H

#include "tree.h"

int tree_dump (tree_t *tree);
int tree_error (tree_t *tree);
void tie_gv_nodes (node_t *node);
void print_gv_nodes (node_t *node);
void tree_graph (tree_t *tree, const char *dot_file_name, const char* png_name);

#endif /*TREE_DUMP_H*/
