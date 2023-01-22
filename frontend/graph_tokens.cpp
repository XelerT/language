#include <stdio.h>
#include <assert.h>
#include "..\include\graph_tokens.h"

static const char digraph_atr_sample[] = "digraph %s {                                  \
                                                \ngraph [dpi = %d];                     \
                                                \nsplines = %s;                         \
                                                \n";

static const char node_atr_sample[] = "\nnode%p[shape = \"%s\", style = \"%s\",                                          \
		                                \nheight = %d, width = %d, fixedsize = %s,                               \
		                                \nfillcolor = \"%s\",                                                    \
		                                \nfontsize = %d,                                                         \
		                                \npenwidth = %d                                                          \
		                                \n];\n";

static const char edge_atr_sample[] = "\nedge[penwidth = %d, color = \"%s\"];";

static const char node_sample_str[]  = "node%p [shape = record label = \"{\\%s | %s}\"];\n";
static const char node_sample_dbl[]  = "node%p [label = \"%lg\"];\n";
static const char node_sample_c[]    = "node%p [label = \"%c\"];\n";
static const char node_sample_dec[]  = "node%p [label = \"%d\"];\n";

static const char nodes_tie_atr[]    = "node%p -> node%p [color = %s];\n";


static FILE *dot_file = nullptr;
#define gv_print(form,...) fprintf(dot_file, form, __VA_ARGS__)

void arr_graph (tokens_t *tokens, const char *dot_file_name, const char* png_name)
{
        if (!tokens) {
                log(1, "Tokens pointer is null.");
                return;
        }
        /*static FILE * */dot_file = fopen(dot_file_name, "w");
        if (!dot_file) {
                log(1, "Dot file pointer is null.");
                return;
        }
        digraph_t G = {};

        gv_print(digraph_atr_sample, "G", G.dpi, G.splines);    /*graph_name, dpi, splines, edge_sample, node_sample*/
        fprintf(dot_file, "{ rank = same;\n");
        print_gv_arr_nodes(tokens);
        tie_gv_arr_nodes(tokens);

        log_pic(3, "<img src=\"%s\" width=\"1060\" alt=\"%s\">", png_name, png_name);

        fprintf(dot_file, "\n}\n}");
        fclose(dot_file);
}

#define KW(word,type,number)    /*if (type == DATA_TYPE) {*/                                                                \
                                        if (number == node->sub_type)                                                   \
                                                gv_print(node_sample_var, node, node->name, #word, node->val);          \
                                // }

void print_gv_arr_nodes (tokens_t *tokens)
{
        token_arg_t *node = nullptr;
        for (size_t i = 0; i < tokens->size; i++) {
                node = tokens->tok_args + i;
                gv_print(node_atr_sample, node, node->atr.shape, node->atr.style, node->atr.height,
                         node->atr.width, node->atr.fixedsize, node->atr.fillcolor,node->atr.fontsize,
                         node->atr.penwidth);

                const char node_sample_data_t[]  = "node%p [shape = record label = \"{%s | {%s | %d}}\"];\n";

                if (node->type == STAFF && node->sub_type == RETURN) {
                        gv_print(node_sample_str, node, node->name, "return");
                        log(1, "Paint return");
                } else if (node->type == DATA_TYPE) {
                        gv_print(node_sample_data_t, node, node->name, "data type", node->sub_type);
                        log(1, "Paint token with DATA_TYPE type");
                } else if (node->type == NAME) {
                        gv_print(node_sample_str, node, node->name, "name type");
                        log(1, "Paint token with name type");
                } else if (node->type == NUMBER) {
                        gv_print(node_sample_dec, node, node->val);
                } else if (node->type == CYCLE && node->sub_type == WHILE) {
                        gv_print(node_sample_str, node, node->name, "while");
                } else if (node->type == OPERATOR && node->sub_type == IF) {
                        gv_print(node_sample_str, node, node->name, "if");
                } else if (node->type == OPERATOR && node->sub_type == ELSE) {
                        gv_print(node_sample_str, node, node->name, "else");
                } else if (node->type == OP_C_BRACKET) {
                        gv_print(node_sample_str, node, node->name, "op_curve brack");
                } else if (node->type == CL_C_BRACKET) {
                        gv_print(node_sample_str, node, node->name, "cl_curve brack");
                } else if (node->type == ASSIGNMENT) {
                        gv_print(node_sample_str, node, node->name, "assignment");
                } else {
                        gv_print(node_sample_str, node, node->name, "?-type");
                }
        }
}

#undef KW

void tie_gv_arr_nodes (tokens_t *node)
{
        for (size_t i = 0; i < node->size - 1; i++) {
                gv_print(nodes_tie_atr, node->tok_args + i, node->tok_args + i + 1, "black");
        }
}
