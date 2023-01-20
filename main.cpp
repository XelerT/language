#include <stdio.h>
#include <stdlib.h>
#include "tree\text_tree.h"
#include "tree\tree_dump.h"
#include "include\lexer.h"
#include "include\graph_tokens.h"
#include "tree\token_tree.h"
#include "backend.h"

int main ()
{
        log_init("logs.html");
        tree_t tree = {};
        tree_ctor(&tree);

        tokens_t tokens = {};
        tokens_ctor(&tokens, 100);
        $
        get_tokens(&tokens, "input.txt");
        $
        arr_graph (&tokens, "tokens_graph.dot", "tokens.png");
        system("make graph_tokens");
        size_t tp = 0;
        tree.root = get_g(&tokens, &tp, &tree);
        tree_graph(&tree, "tree_graph.dot", "tree.png");
        system("make graph_tree");
        create_asm (&tree, "asm_code.txt");
        $
        log_dtor();
        return 0;
}
