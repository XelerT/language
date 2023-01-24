#ifndef TREE_2_TEXT
#define TREE_2_TEXT

#include "tree.h"
#include "text_tree.h"

int write_node_data        (FILE *text_tree, node_t *node);
int write_node             (FILE *text_tree, node_t *node, int n_tabs);
int tree_2_text            (tree_t *tree, const char* txt_file_name);
void print_tabs            (FILE *text_tree, int n_tabs);

int text_2_tree            (tree_t *tree);
int read_node              (char *buf, size_t *counter, node_t *node, int prev_has_right);
char read_node_data        (char *buf, size_t *counter, node_t *node);
int read_name              (char *dest, char *src, size_t *counter);
int read_number            (char *buf, size_t *counter);
int read_special_node_data (char *buf, size_t *counter, node_t *node);

void free_buf (char *buf);

#endif /*TREE_2_TEXT*/
