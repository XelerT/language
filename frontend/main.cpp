#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "..\tree\text_tree.h"
#include "..\tree\tree_dump.h"
#include "..\include\lexer.h"
#include "..\include\graph_tokens.h"
#include "..\tree\token_tree.h"
#include "..\backend\backend.h"
#include "..\tree\tree_2_text.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"

int main (int argc, char *argv[])
{
        log_init("output_utils\\logs_frontend.html");

        if (argc != 3) {
                log(3, "<span style = \"color: red; font-size:16px;\">Error with input data. Args: \n\t\t\t 1st \"%s\"\n\t\t\t 2st: \"%s\", argc: %d</span>",
                        argv[1], argv[2], argc);
                fprintf(stderr, "You need to enter 2 arguments.\n");
                return NULL_FILE;
        }
        char  input_file_name[MAX_NAME_LENGTH] = {'\0'};
        char output_file_name[MAX_NAME_LENGTH] = {'\0'};
        strcpy(input_file_name,  argv[2]);
        strcpy(output_file_name, argv[1]);

        tree_t tree = {};
        tree_ctor(&tree);

        tokens_t tokens = {};
        tokens_ctor(&tokens, 100);

        get_tokens(&tokens, input_file_name);

        // arr_graph (&tokens, "output_utils\\tokens_graph.dot", "tokens.png");
        // system("make graph_tokens");

        size_t tp = 0;
        tree.root = get_g(&tokens, &tp, &tree);

        tree_graph(&tree, "output_utils\\tree_graph.dot", "front_tree.png");
        system("make graph_tree IMG_PATH=output_utils\\front_tree.png");

        tree_2_text(&tree, "tree.txt");

        tree_dtor(&tree);
        free(tokens.tok_args);

        log_dtor();
        return 0;
}