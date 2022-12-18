#include <stdio.h>
#include <assert.h>
#include "tree_dump.h"

int tree_dump (tree_t *tree)
{
        const char *tree_errors[] = {
                "Tree pointer is null.",
                "Node pointer in tree is null.",
        };

        FILE *dump_file = fopen("dump.html", "w");

        int errors = tree_error(tree);
        fprintf(dump_file, "<pre>\n");

        if (!errors) {
                fprintf(dump_file, "</pre>\n");
                return 0;
        }

        for (int i = 0; (errors << i) != 0; i++) {
                switch (errors & ~(~0u << (i + 1))) {                                                                      //1111 1111
                case NULL_TREE_PTR:                                                                                     //1111 1100
                        fprintf(dump_file, "<p style=\"color:red;\">%s</p></pre>\n", tree_errors[i]);                  //0000 0011
                        return NULL_TREE_PTR;                                                                          //&0000 1010
                case NULL_ROOT_PTR:                                                                                     //0000 0010
                        fprintf(dump_file, "<p style=\"color:red;\">%s</p></pre>\n", tree_errors[i]);
                        return NULL_ROOT_PTR;
                default:
                        fprintf(stderr, "SMTH WENT WRONG: FILE %s, LINE: %d", __FILE__, __LINE__);
                }
        }

        fprintf(dump_file, "</pre>\n");

        return errors;
}

int tree_error (tree_t *tree)
{
        int errors = 0;
        if (tree == nullptr)
                errors |= NULL_TREE_PTR;
        if (tree->root == nullptr)
                errors |= NULL_ROOT_PTR;

        return errors;
}

//----------------------------------------------------------------------------------------------------------------------------

static const char digraph_atr_sample[] = "digraph %s {                                  \
                                                \ngraph [dpi = %d];                     \
                                                \nsplines = %s;                         \
                                                \n";

static const char node_atr_sample[] = "\nnode%p[shape = \"%s\", style = \"%s\",                                            \
		                                \nheight = %d, width = %d, fixedsize = %s,                               \
		                                \nfillcolor = \"%s\",                                                    \
		                                \nfontsize = %d,                                                         \
		                                \npenwidth = %d                                                          \
		                                \n];\n";

static const char edge_atr_sample[] = "\nedge[penwidth = %d, color = \"%s\"];";

static const char node_sample_str[]  = "node%p [label = \"%s\"];\n";
static const char node_sample_dbl[]  = "node%p [label = \"%lg\"];\n";
static const char node_sample_dec[]  = "node%p [label = \"%c\"];\n";

static const char nodes_tie_atr[] = "node%p -> node%p [color = %s];\n";


static FILE *dot_file = nullptr;
#define gv_print(form,...) fprintf(dot_file, form, __VA_ARGS__)

void tree_graph (tree_t *tree, const char *dot_file_name, const char* png_name)
{
        if (!tree) {
                fprintf(stderr, "Tree pointer is null. \nIn File: \n\t%s \nLine: \n\t%d.\n", __FILE__, __LINE__);
                return;
        }
        /*FILE * */dot_file = fopen(dot_file_name, "w");
        if (!dot_file) {
                fprintf(stderr, "Dot file pointer is null. \nIn File: \n\t%s \nLine: \n\t%d.\n", __FILE__, __LINE__);
                return;
        }
        digraph_t G = {};

        gv_print(digraph_atr_sample, "G", G.dpi, G.splines);    /*graph_name, dpi, splines, edge_sample, node_sample*/
        print_gv_nodes(tree->root);
        tie_gv_nodes(tree->root);

        fprintf(dot_file, "\n}");
        fclose(dot_file);
        log_pic(3, "<img src=\"%s\" width=\"460\" alt=\"%s\">", png_name, png_name);
}

void print_gv_nodes (node_t *node)
{
        if (node->left)
                print_gv_nodes(node->left);
        if (node->right)
                print_gv_nodes(node->right);

        gv_print(node_atr_sample, node, node->atr.shape, node->atr.style, node->atr.height,
                 node->atr.width, node->atr.fixedsize, node->atr.fillcolor,node->atr.fontsize,
                 node->atr.penwidth);

        // if (node->type == VARIABLE || node->type == CONST)
        //         gv_print(node_sample_dec, node, node->data.var);
        // else if (node->type == OPERATOR)
        //         gv_print(node_sample_dec, node, node->data.op);
        if (node->type == NUMBER)
                gv_print(node_sample_dec, node, node->data);
        else
                gv_print(node_sample_str, node, node->name);
}


// conect

void tie_gv_nodes (node_t *node)
{
        if (node->left)
                tie_gv_nodes(node->left);
        if (node->right)
                tie_gv_nodes(node->right);
        if (node->left) {
                gv_print(nodes_tie_atr, node, node->left, "black");
        }
        if (node->right) {
                gv_print(nodes_tie_atr, node, node->right, "red");

        }
}

