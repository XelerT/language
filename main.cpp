#include <stdio.h>
#include "tree\text_tree.h"
#include "tree\tree_dump.h"
#include "lexer.h"
#include "graph_tokens.h"

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
        // size_t char_count = 0;
        // tree.root = get_g(text.buf, &char_count, &tree);
        // tree_graph(&tree, "tree_graph.dot");

        tokens_t tokens = {};
        $
        tokens_ctor(&tokens, 100);
        $
        // log_dtor();

        get_tokens(&tokens, "input.txt");
        $
        arr_graph (&tokens, "tokens_graph.dot", "graph.png");
        $
        log_dtor();
        return 0;
}
