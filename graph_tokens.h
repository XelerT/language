#ifndef GRAPH_TOKENS_H
#define GRAPH_TOKENS_H

#include "lexer.h"
#include "tree\tree_dump.h"
#include "log\log.h"

void arr_graph (tokens_t *tokens, const char *dot_file_name, const char *png_name);
void print_gv_arr_nodes (tokens_t *tokens);
void tie_gv_arr_nodes (tokens_t *node);

#endif /*GRAPH_TOKENS_H*/
