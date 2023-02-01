#ifndef TOKEN_TREE_H
#define TOKEN_TREE_H

#include "tree.h"
#include "../include/tokens.h"

node_t* get_g          (const tokens_t *tokens, size_t *tp, tree_t *tree);
node_t* get_end_line   (const tokens_t *tokens, size_t *tp, tree_t *tree);
node_t* hub            (const tokens_t *tokens, size_t *tp, tree_t *tree);
node_t* get_div_mul    (const tokens_t *tokens, size_t *tp, tree_t *tree);
node_t* get_sub_add    (const tokens_t *tokens, size_t *tp, tree_t *tree);
node_t* get_if         (const tokens_t *tokens, size_t *tp, tree_t *tree);
node_t* get_else       (const tokens_t *tokens, size_t *tp, tree_t *tree);
node_t* get_while      (const tokens_t *tokens, size_t *tp, tree_t *tree);
node_t* get_conj       (const tokens_t *tokens, size_t *tp, tree_t *tree);
node_t* get_brackets   (const tokens_t *tokens, size_t *tp, tree_t *tree);
node_t* get_next_elem  (const tokens_t *tokens, size_t *tp, tree_t *tree);
node_t* get_assign     (const tokens_t *tokens, size_t *tp, tree_t *tree);
node_t* get_curv_brack (const tokens_t *tokens, size_t *tp, tree_t *tree);
node_t* get_func_init  (const tokens_t *tokens, size_t *tp, tree_t *tree);
node_t* get_func       (const tokens_t *tokens, size_t *tp, tree_t *tree);
node_t* get_n          (const tokens_t *tokens, size_t *tp, tree_t *tree);

void edit_temp  (node_t *temp_node, token_arg_t *token);
void copy_token (token_arg_t *dest_token, token_arg_t *src_token);

#endif /*TOKEN_TREE_H*/
