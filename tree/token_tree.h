#ifndef TOKEN_TREE_H
#define TOKEN_TREE_H

#include "tree.h"
#include "..\include\tokens.h"

node_t* get_g (const tokens_t *tokens, size_t *tp, tree_t *tree);
node_t* get_e (const tokens_t *tokens, size_t *tp, tree_t *tree);
node_t* get_t (const tokens_t *tokens, size_t *tp, tree_t *tree);
node_t* get_p (const tokens_t *tokens, size_t *tp, tree_t *tree);
node_t* get_n (const tokens_t *tokens, size_t *tp, tree_t *tree);

void edit_temp (node_t *temp_node, token_arg_t *token);
void copy_token(token_arg_t *dest_token, token_arg_t *src_token);

#endif /*TOKEN_TREE_H*/
