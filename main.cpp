#include <stdio.h>
#include "tree\text_tree.h"
#include "tree\tree_dump.h"
#include "include\lexer.h"
#include "include\graph_tokens.h"
#include "tree\token_tree.h"

int main ()
{
        log_init("logs.html");
        tree_t tree = {};
        text_t text = {};
        tree_ctor(&tree);
        // FILE *input = fopen("input.txt", "r");
        // int error = get_text(input, &text, "input.txt");
        // log(1, "help me pls, i'm dying");
        // if (error)
        //         return error;
        // replace_n(&text);
        // divide_text(&text);

        tokens_t tokens = {};
        tokens_ctor(&tokens, 100);

        // printf("ў\n");

        get_tokens(&tokens, "input.txt");
        $
        arr_graph (&tokens, "tokens_graph.dot", "tokens.png");
        size_t tp = 0;
        tree.root = get_g(&tokens, &tp, &tree);
        tree_graph(&tree, "tree_graph.dot", "tree.png");
        $
        log_dtor();
        return 0;
}
